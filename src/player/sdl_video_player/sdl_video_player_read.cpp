//
// Created by leo on 24-10-10.
//
#include <algorithm>
#include "player/sdl_video_player/sdl_video_player.h"
#include "util/ff_util.h"
#ifdef __cplusplus
extern "C" {
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
}
#else
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#endif

#define MAX_FRAME_DURATION_FOR_DISCON 10.0
#define MAX_FRAME_DURATION_FOR_CON 3600.0

void SDLVideoPlayer::determineStream() {
  auto& stIndex = videoInfo.streamIndex;
  // 差不多nb_streams应该不会太大，所以就用uint16_t了
  assert(videoInfo.fmtCtx->nb_streams <= std::numeric_limits<uint16_t>::max());
  for (uint16_t i =0; i < videoInfo.fmtCtx->nb_streams;++i) {
    AVStream* st = videoInfo.fmtCtx->streams[i];
    AVMediaType ty = st->codecpar->codec_type;
    st->discard = AVDISCARD_ALL;
    if (ty > AVMediaType::AVMEDIA_TYPE_UNKNOWN && settings.wantedStreamSpec[ty]&& stIndex[ty]) {
      if (avformat_match_stream_specifier(videoInfo.fmtCtx.get(), st, settings.wantedStreamSpec[ty]) > 0) {
        stIndex[ty] = i; // 对于某一类型的流，选择第一个符合筛选条件的流，若没有，仍是-1
      }
    }
  }

  // 如果确实提供了某种类型的流的筛选条件，但是没有找到，就警告
  for (uint16_t i =0; i < AVMEDIA_TYPE_NB; ++i) {
    if (settings.wantedStreamSpec[i]&& stIndex[i] == -1) {
      auto desc = ErrorDesc::from(
        ExceptionType::WantedStreamNotFound,
        std::string("Can't find this type of stream: ") + av_get_media_type_string(static_cast<AVMediaType>(i)) + "mathing your spec" + settings.wantedStreamSpec[i]
      );
      PlayerLogger::log(desc);
    }
  }

  if (!settings.disableVid) {
    stIndex[AVMEDIA_TYPE_VIDEO] = av_find_best_stream(videoInfo.fmtCtx.get(),
                                                     AVMEDIA_TYPE_VIDEO,
                                                     stIndex[AVMEDIA_TYPE_VIDEO],
                                                     -1,
                                                     nullptr,
                                                     0);
  }
  if (!settings.disableAud) {
    stIndex[AVMEDIA_TYPE_AUDIO] = av_find_best_stream(videoInfo.fmtCtx.get(),
                                                       AVMEDIA_TYPE_AUDIO,
                                                       stIndex[AVMEDIA_TYPE_AUDIO],
                                                       stIndex[AVMEDIA_TYPE_VIDEO], // 此音频流应该和已经找到的视频流同步
                                                       nullptr,
                                                       0);
  }
  if (!settings.disableVid && !settings.disableSub) {
    /*在related stream中，如果video stream存在，那么subtitle stream应该和video stream同步，否则，和audio stream同步*/
    stIndex[AVMEDIA_TYPE_SUBTITLE] = av_find_best_stream(videoInfo.fmtCtx.get(),
                                                         AVMEDIA_TYPE_SUBTITLE,
                                                         stIndex[AVMEDIA_TYPE_SUBTITLE],
                                                         stIndex[AVMEDIA_TYPE_AUDIO] >= 0 ? stIndex[AVMEDIA_TYPE_AUDIO] : stIndex[AVMEDIA_TYPE_VIDEO],
                                                         nullptr,
                                                         0);
  }
}

std::optional<ErrorDesc> SDLVideoPlayer::openStreamComponent(AVMediaType type, uint16_t streamIndex) {
  assert(streamIndex < videoInfo.fmtCtx->nb_streams); // 因为调用这个函数之前已经确定了这个streamIndex是合法的，责任在调用者
  int ret;
  AVStream* st = videoInfo.fmtCtx->streams[streamIndex];

  AVCodecContext* codecCtx = avcodec_alloc_context3(nullptr);
  if ( !codecCtx ) return ErrorDesc::from(ExceptionType::MemoryAllocFailed, "Can't allocate AVCodecContext");
  std::unique_ptr<AVCodecContext, AVCodecContextDeleter> codecCtxPtr(codecCtx); // 防止内存泄漏

  ret = avcodec_parameters_to_context(codecCtx, st->codecpar); // 从codecpar中拷贝信息到codecCtx
  if (ret < 0) return ErrorDesc::from(ExceptionType::FFmpegSetFailed, std::string("Can't init codec context for ") + std::to_string(streamIndex));

  codecCtx->pkt_timebase = st->time_base;

  const AVCodec* codec = nullptr;
  const std::string* specDecoderName;
  int32_t* lastStInd = &playState.lastAudStInd;
  switch (type) {
    case AVMEDIA_TYPE_VIDEO:
      specDecoderName = &settings.videoDecoderName;
      lastStInd = &playState.lastVidStInd;
      break;
    case AVMEDIA_TYPE_AUDIO:
      specDecoderName = &settings.audioDecoderName;
      break;
    case AVMEDIA_TYPE_SUBTITLE:
      specDecoderName = &settings.subDecoderName;
      lastStInd = &playState.lastSubStInd;
      break;
    default:
      return ErrorDesc::from(ExceptionType::InvalidArgument, "Unsupported media type");
  }
  if (!specDecoderName->empty()) {
    codec = avcodec_find_decoder_by_name(specDecoderName->c_str());
    if (!codec) {
      if (settings.forceSpecifiedDecoder) {
        return ErrorDesc::from(ExceptionType::FFmpegSetFailed, std::string("Can't find decoder: ") + *specDecoderName);
      }
      codec = avcodec_find_decoder(codecCtx->codec_id);
      if (!codec) {
        return ErrorDesc::from(ExceptionType::FFmpegSetFailed, std::string("Try to use default decoder, but can't find decoder: ") + avcodec_get_name(codecCtx->codec_id));
      }
      // 说明没有使用指定的解码器，而是使用了默认的解码器， 输出warning
      PlayerLogger::log(ErrorDesc::from(ExceptionType::UseOtherDecoder, std::string("Try to use default decoder, but use decoder: ") + avcodec_get_name(codecCtx->codec_id)));
    }
    codecCtx->codec_id = codec->id; // 这里是为了保证codec_id和codec->id一致
  }else {
    codec = avcodec_find_decoder(codecCtx->codec_id);
    if (!codec) {
      return ErrorDesc::from(ExceptionType::FFmpegSetFailed, std::string("Can't find decoder: ") + avcodec_get_name(codecCtx->codec_id));
    }
  }
  // 这里说明找到了解码器, 设置lastStreamInd
  *lastStInd = streamIndex;
  // 尝试设置的分辨率等级
  if (settings.lowres<=codec->max_lowres) {
    codecCtx->lowres = settings.lowres;
  }else {
    codecCtx->lowres = codec->max_lowres;
    PlayerLogger::log(ErrorDesc::from(ExceptionType::UnsupportedLowResLevel, "Unsupported lowres level, use max lowres level: " + std::to_string(codec->max_lowres)));
  }
  // opts for this stream
  AVDictionary* opts = settings.filterOpts(codec, codecCtx,videoInfo.fmtCtx.get(), st);
  // fast mode
  if (settings.decodeFastMode) codecCtx->flags2 |= AV_CODEC_FLAG2_FAST;
  // threads num 已经在settings.filterOpts这一步设置了
  // lowres 已经在settings.filterOpts这一步设置了
  av_dict_set(&opts, "flags","+copy_opaque",AV_DICT_MULTIKEY);
  // open codec
  ret = avcodec_open2(codecCtx, codec, &opts);
  std::unique_ptr<AVDictionary, AVDictionaryDeleter> optsPtr(opts);
  if (ret < 0) {
    return ErrorDesc::from(ExceptionType::FFmpegSetFailed, std::string("Can't open codec: ") + avcodec_get_name(codecCtx->codec_id));
  }
  // 检查设置的opts是否都被使用了
  std::vector<AVDictionaryEntry*> optlist = FFUtil::getAllEntries(opts);
  if (!optlist.empty()) {
    // 释放opts
    return ErrorDesc::from(ExceptionType::UnusedFFOption, "Some options are not used: " + FFUtil::stringifyAVDictionary(optlist));
  }
  // 主动释放opts
  optsPtr.reset();
  playState.eof = false;
  st->discard = AVDISCARD_DEFAULT; // 默认情况下，仅丢弃useless packets(like 0 size packets in avi)

  // 开始分不同类型
  switch (type) {
    case AVMEDIA_TYPE_AUDIO:
    {
      AudioParams& filterParams = mediaFilterInfo.audioFilterParamsSrc;
      AVFilterContext* buffersinkCtx = nullptr;
      //TODO: 这一步的set大概率不会失败，，但其中对于ch_layout的copy也可能会失败，之后正在回头看这一步吧
      mediaFilterInfo.setAudFilterParamsSrc(codecCtx->sample_rate, &codecCtx->ch_layout, codecCtx->sample_fmt);
      // 第一次调用
      auto err = mediaFilterInfo.configAudioFilter(settings.swrOpts, settings.audioFilterGraphStr, settings.filterThreadsNum, false);
      if (err) return err; // 如果配置失败，直接返回错误信息
    }
      break;
    case AVMEDIA_TYPE_VIDEO:
      break;
    case AVMEDIA_TYPE_SUBTITLE:
      break;
    default:
      return ErrorDesc::from(ExceptionType::InvalidArgument, "Unsupported media type");
  }

  return std::nullopt;
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
  determineStream();
  if (videoInfo.streamIndex[AVMEDIA_TYPE_VIDEO] >= 0) {
    AVStream* st = fmtCtx->streams[videoInfo.streamIndex[AVMEDIA_TYPE_VIDEO]];
    AVCodecParameters* codecpar = st->codecpar;
    AVRational sar = av_guess_sample_aspect_ratio(fmtCtx, st, nullptr);
    std::tie(playState.defaultPicWidth, playState.defaultPicHeight) = FFUtil::getPictureDisplayRect(codecpar->width,
                                                                                                            codecpar->height,
                                                                                                            sar,
                                                                                                            settings.windowWidth,
                                                                                                            settings.windowHeight);

  }
  // 开始打开各个流
  if (videoInfo.streamIndex[AVMEDIA_TYPE_VIDEO] >= 0) {
    auto err = openStreamComponent(AVMEDIA_TYPE_VIDEO, videoInfo.streamIndex[AVMEDIA_TYPE_VIDEO]);
  }
  if (videoInfo.streamIndex[AVMEDIA_TYPE_AUDIO] >= 0) {
    auto err = openStreamComponent(AVMEDIA_TYPE_AUDIO, videoInfo.streamIndex[AVMEDIA_TYPE_AUDIO]);
  }
  av_packet_free(&pkt);
}
