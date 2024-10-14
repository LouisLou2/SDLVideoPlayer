//
// Created by leo on 24-10-9.
//
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
  const std::optional<SDLVidPlayerSettings>& setting
):
settings(
  setting.value_or(
    SDLVidPlayerSettings(
      SDLVideoPlayer::programName + " " + video_path,
      true,
      false,
      false,
      false
      )
    )
),
cacheCollection(
  PIC_CAP_MAX, true,
  SAMPLE_CAP_MAX, false,
  SUB_CAP_MAX, false
){
  videoInfo.originalUrl = video_path;
}

void SDLVideoPlayer::initAv() {
  avdevice_register_all(); // 高版本的ffmpeg这一步是可选的
  // 初始化网络
  if (settings.enableNetwork) avformat_network_init();
}