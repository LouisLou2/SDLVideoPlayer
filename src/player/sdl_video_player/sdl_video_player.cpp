//
// Created by leo on 24-9-29.
//
#include <filesystem>
#include <functional>
#include <fstream>
#include <optional>
#include <vector>
#include <limits>

#include "entity/error_desc.h"
#include "util/url_util.h"
#include "util/calc.h"
#include "util/ff_util.h"
#include "util/logger/player_logger.h"
#include "player/sdl_video_player/sdl_video_player.h"
#include "util/mem/ff_mem.h"

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
    std::ifstream videoInFile(videoInfo.originalUrl, std::ios::in | std::ios::binary);
    if (!videoInFile.is_open()) {
      throw ErrorDesc::from(ExceptionType::FileUnreadable, std::string("Can't open file: ") + videoInfo.originalUrl);
    }else {
      // 关闭文件
      videoInFile.close();
    }
  }
  // 等到play函数调用时才初始化
  initAv();
  initSDL();
  openStream();
  coordinator.readThread.join();
}
