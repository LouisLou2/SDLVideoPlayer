//
// Created by leo on 24-10-9.
//
#include "const/struct_config.h"
#include "player/play_envs.h"
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


SDLVideoPlayer::SDLVideoPlayer(
  const std::string& video_path,
  const std::optional<const SDLVidPlayerSettings*>& setting
): settings(setting.has_value() && setting.value() ? *(setting.value()) : SDLVidPlayerSettings(programName)),
   audDecodeRegin(SAMPLE_CAP_MAX, false, playState, settings.presentForm),
   vidDecodeRegin(PIC_CAP_MAX, true,SUB_CAP_MAX, false, playState),
   playState(this->settings.playSpeed) {
  videoInfo.originalUrl = video_path;
}

void SDLVideoPlayer::initAv() {
  avdevice_register_all(); // 高版本的ffmpeg这一步是可选的
  // 初始化网络
  if (settings.enableNetwork) avformat_network_init();
}
