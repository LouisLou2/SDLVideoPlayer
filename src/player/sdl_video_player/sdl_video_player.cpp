//
// Created by leo on 24-9-29.
//
#include <filesystem>
#include <functional>
#include <optional>
#include <vector>
#include <limits>
#include <bits/ranges_algo.h>

#include "entity/error_desc.h"
#include "util/url_util.h"
#include "util/calc.h"
#include "util/ff_util.h"
#include "util/logger/player_logger.h"
#include "player/sdl_video_player/sdl_video_player.h"

#ifdef __cplusplus
extern "C" {
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
}
#else
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#endif

#define MIN_STREAM_SIZE 1024
#define MAX_FRAME_DURATION_FOR_DISCON 10.0
#define MAX_FRAME_DURATION_FOR_CON 3600.0

namespace fs = std::filesystem;


SDLVideoPlayer::SDLVideoPlayer(
  const std::string& video_path,
  const std::optional<SDLVidPlayerSettings>& setting
):
settings(
  setting.value_or(
    SDLVidPlayerSettings(
      SDLVideoPlayer::programName + " " + video_path,
      true,
      false,
      false,
      false,
      false,
      DEF_WIN_WIDTH,
      DEF_WIN_HEIGHT
      )
    )
),
vFrameq(PIC_CAP_MAX, true),
aFrameq(SAMPLE_CAP_MAX, false),
sFrameq(SUB_CAP_MAX, false)
{
  videoInfo.originalUrl = video_path;
}

void SDLVideoPlayer::initAv() {
  avdevice_register_all(); // 高版本的ffmpeg这一步是可选的
  // 初始化网络
  if (settings.enableNetwork) avformat_network_init();
}

void SDLVideoPlayer::initSDL() {
  uint32_t flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
  if (settings.disableAud) flags &= ~SDL_INIT_AUDIO;
  if (settings.disableVid) flags &= ~SDL_INIT_VIDEO;
  if (SDL_Init(flags)) {
    // 初始化失败
    throw ErrorDesc::from(ExceptionType::SDLInitFailed, SDL_GetError());
  }
  // 使SDL不处理这些事件，以便我们自己处理
  SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
  SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

  if (settings.disableVid) return;

  uint32_t sdlFlags = SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;
  if (settings.alwaysOnTop) sdlFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
  if (settings.borderless) sdlFlags |= SDL_WINDOW_BORDERLESS;
  // 禁用窗口合成器
  SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
  // 设置渲染器缩放质量
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  // 创建窗口
  window.reset(SDL_CreateWindow(
    settings.windowTitle.c_str(),
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    settings.windowWidth,
    settings.windowHeight,
    sdlFlags
  ));
  if (window) {
    // 创建渲染器
    renderer.reset(
    SDL_CreateRenderer(window.get(),
      -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
      )
    );
    if (!renderer) {
      PlayerLogger::log(ErrorDesc::from(ExceptionType::SDLHardwareAccelerationFailed, SDL_GetError()));
      // 如果创建失败，尝试不使用硬件加速
      renderer.reset(
        SDL_CreateRenderer(
        window.get(),
        -1,
        0)
      );
    }
  }
  if (renderer) {
    // 获取渲染器信息
    if (!SDL_GetRendererInfo(renderer.get(), &rendererInfo))
      PlayerLogger::log(LogLevel::Info, std::string("Renderer Initialized: ") + rendererInfo.name);
  }
  // 检查一系列创建是否成功
  if (!window || !renderer || !rendererInfo.num_texture_formats) {
    throw ErrorDesc::from(ExceptionType::SDLComponentInitFailed, SDL_GetError());
  }
}

void SDLVideoPlayer::read() {
  int ret;
  // playState.eof默认是false，这里不用修改了
  AVPacket* pkt = av_packet_alloc();
  if (!pkt) throw ErrorDesc::from(ExceptionType::MemoryAllocFailed, "Can't allocate AVPacket");

  AVFormatContext* fmtCtx = avformat_alloc_context();
  if (!fmtCtx) throw ErrorDesc::from(ExceptionType::MemoryAllocFailed, "Can't allocate AVFormatContext");
  // 下面的两个语句简直恶心，但是为了配合ffmpeg的设计，不得不这样写
  fmtCtx->interrupt_callback.callback = &SDLPlayState::getAborted;
  fmtCtx->interrupt_callback.opaque = &playState;
  // 得到videoInfo.fmtCtx

  AVDictionary* fmtOpts = settings.getFormatOpt();
  if (!av_dict_get(fmtOpts, "scan_all_pmts", nullptr, AV_DICT_MATCH_CASE)) {
    av_dict_set(&fmtOpts, "scan_all_pmts", "1", 0);
  }
  ret = avformat_open_input(&fmtCtx, videoInfo.originalUrl.c_str(), videoInfo.iformat, &fmtOpts);
  // 释放fmtOpts,尽早释放，防止出现什么异常导致内存泄漏
  av_dict_free(&fmtOpts);
  if (ret < 0) throw ErrorDesc::from(ExceptionType::BrokenStream, "Can't open stream: " + videoInfo.originalUrl);
  videoInfo.fmtCtx.reset(fmtCtx); // 使用unique_ptr管理fmtCtx, 但是接下来的使用还是用fmtCtx，方便

  if (settings.genPts) fmtCtx->flags |= AVFMT_FLAG_GENPTS;

  if (settings.findStreamInfo) {
    // 将每个流需要的dict option都设置好
    std::vector<AVDictionary*> optlist = settings.getCodecOpts(fmtCtx);
    AVDictionary** optlistPtr = optlist.data();
    ret = avformat_find_stream_info(fmtCtx, optlistPtr);
    // 使用完了，释放：foreach语句
    std::ranges::for_each(optlist, [](AVDictionary*& dict) { av_dict_free(&dict); });
    // 检查是否成功
    if (ret < 0) throw ErrorDesc::from(ExceptionType::BrokenStream, "Can't find stream info: " + videoInfo.originalUrl);
  }

  if(fmtCtx->pb) fmtCtx->pb->eof_reached = 0; // 表示未到达末尾 //TODO: 这里对于网络流可能有问题

  if(settings.specifiedSeekType) settings.seekType = FFUtil::decideSeekType(fmtCtx->iformat);

  playState.maxFrameDuration = (FFUtil::hasDiscontinuousTS(fmtCtx->iformat)) ? MAX_FRAME_DURATION_FOR_DISCON : MAX_FRAME_DURATION_FOR_CON;

  // settings.playOffsetSec和playState.playOffsetSec初始均是0
  if (settings.playOffsetSec) {
    playState.playOffset = settings.playOffsetSec * AV_TIME_BASE;
    // 如果fmtCtx->start_time == AV_NOPTS_VALUE，直接会忽略就行了吗？这貌似意味着fmtCtx->start_time很奇怪，这个流可能有问题
    uint64_t fromTs = ( fmtCtx->start_time == AV_NOPTS_VALUE ) ? playState.playOffset : playState.playOffset + fmtCtx->start_time; // 开始播放的pts
    // seek in file
    // avformat_seek_file较为复杂，可以参考note/ff_api.md中的解释
    ret = avformat_seek_file(fmtCtx, -1, INT64_MIN, fromTs, INT64_MAX, 0);
    // 如果seek失败，视作warning, 不抛出异常
    if (ret < 0) {
      PlayerLogger::log(ErrorDesc::from(ExceptionType::PlaySeekFailed, std::string("First seek failed for file: ") + videoInfo.originalUrl));
    }
  }

  videoInfo.isRealTime = FFUtil::isRealTime(fmtCtx);

  if (settings.showStatus) {
    av_dump_format(fmtCtx, 0, videoInfo.originalUrl.c_str(), 0);
  }




  av_packet_free(&pkt);
}

void SDLVideoPlayer::determineStream() {
  // 差不多nb_streams应该不会太大，所以就用uint16_t了
  assert(videoInfo.fmtCtx->nb_streams <= std::numeric_limits<uint16_t>::max());
  for (uint16_t i =0; i < videoInfo.fmtCtx->nb_streams;++i) {
    AVStream* st = videoInfo.fmtCtx->streams[i];
    AVMediaType ty = st->codecpar->codec_type;
    st->discard = AVDISCARD_ALL;
    if (ty > AVMediaType::AVMEDIA_TYPE_UNKNOWN && settings.wantedStreamSpec[ty]&& videoInfo.streamIndex[ty]) {
      if (avformat_match_stream_specifier(videoInfo.fmtCtx.get(), st, settings.wantedStreamSpec[ty]) > 0) {
        videoInfo.streamIndex[ty] = i;
      }
    }
  }
  int x =0;
}


void SDLVideoPlayer::openStream() {
  // PlayerState默认构造函数已经把last以及ind都设为-1了
  // PlayerState默认构造函数已经将窗口坐标和大小设为0了
  if (settings.specifiedInputFormat) {
    videoInfo.iformat = FFUtil::getInputFormat(settings.inputFormat);
    if (!videoInfo.iformat) {
      throw ErrorDesc::from(ExceptionType::UnsupportedFormat, std::string("Unsupported input format: ") + InputFormatEnumUtil::toStr(settings.inputFormat));
    }
  } else {
    videoInfo.iformat = nullptr;
  }
  // 三个frame_queue都在构造函数中初始化了
  // 三个packet_queue的默认构造直接使用即可
  // 三个时钟的默认构造函数已经将所有成员变量初始化了，正式我们想要的，这里就不再修改了
  // playState.audioClockSerial 默认就是-1
  // 虽说settings.volumeWithin100是0-100，但是考虑到浮点运算的精度问题，这里还必须严格要求在0-SDL_MIX_MAXVOLUME之间
  playState.volume = Calc::clip(settings.volumeWithin100 / 100.0 * SDL_MIX_MAXVOLUME, 0, SDL_MIX_MAXVOLUME);
  // playState默认构造函数已经将syncType设置为AudioMaster了
  // playState默认构造函数已经将mute和paused都设置为false了

  // 建立线程
  std::function<void()> task = [this] { read(); };
  coordinator.readThread = std::thread(task);
}

void SDLVideoPlayer::play() {
  // 检查文件是否为本地文件
  videoInfo.protocol = UrlUtil::getProtocolType(videoInfo.originalUrl);
  // 如果是本地文件，检查文件是否存在
  if (videoInfo.protocol == StreamProtocol::File) {
    // 检查文件是否存在
    if (!fs::is_regular_file(videoInfo.originalUrl)) {
      throw ErrorDesc::from(ExceptionType::ResourceNotFound, std::string("Can't find an regular file named: ") + videoInfo.originalUrl);
    }
    // 检查文件是否过小
    if (fs::file_size(videoInfo.originalUrl) < MIN_STREAM_SIZE) {
      throw ErrorDesc::from(ExceptionType::ResourceNotFound, std::string("File is too small: ") + videoInfo.originalUrl);
    }
    // 打开文件检查可读性
    videoInFile.open(videoInfo.originalUrl, std::ios::in | std::ios::binary);
    if (!videoInFile.is_open()) {
      throw ErrorDesc::from(ExceptionType::FileUnreadable, std::string("Can't open file: ") + videoInfo.originalUrl);
    }
  }
  // 等到play函数调用时才初始化
  initAv();
  initSDL();
  openStream();
  coordinator.readThread.join();
}
