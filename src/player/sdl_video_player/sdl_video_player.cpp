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
#include "player/sdl_video_player/sdl_video_player.h"

#include "player/play_envs.h"

#ifdef __cplusplus
extern "C" {
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
}
#else
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#endif

// #define MAX_FRAME_DURATION_FOR_DISCON 10.0
// #define MAX_FRAME_DURATION_FOR_CON 3600.0

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
  playState.setSDLVolumeUsingPercent(settings.volumeWithin100);
  // playState默认构造函数已经将syncType设置为AudioMaster了
  // playState默认构造函数已经将mute和paused都设置为false了

  // 建立线程
  std::function<void()> task = [this] { read(); };
  coordinator.readThread = std::thread(task);
}

void SDLVideoPlayer::play() {
  videoInfo.checkFileAndSetBasic();
  // 明确显示器信息，这个函数可能会抛出异常,它会在第一次调用时初始化
  PlayEnvs::ensureScreenInfoSet();
  // 检查设置是否与硬件兼容
  if (std::optional<ErrorDesc> optErr = settings.checkConflictsWithHardware()) throw optErr.value();
  // 等到play函数调用时才初始化
  initAv();
  // 显示器(使用SDL的)初始化
  if (std::optional<ErrorDesc> optErr = displayer.initDisplayer(settings,playState.presentForm)) throw optErr.value();
  openStream();
  coordinator.readThread.join();
}
