//
// Created by leo on 24-9-29.
//

#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "sdl_cache_collection.h"
#include "sdl_displayer.h"
#include "sdl_media_filter_info.h"
#include "player/sdl_video_player/sdl_thread_coordinator.h"
#include "player/video_player.h"
#include "player/sdl_video_player/sdl_play_state.h"
#include "player/sdl_video_player/sdl_video_basic_info.h"
#include "player/sdl_video_player/sdl_vid_player_setting.h"

class SDLVideoPlayer final : public VideoPlayer {

  static constexpr std::string programName = "SDLVideoPlayer";

  SDLVidPlayerSettings settings;
  VideoBasicInfo videoInfo;
  SDLPlayState playState;
  SDLThreadCoordinator coordinator;
  SDLMediaFilterInfo mediaFilterInfo;
  SDLPlayerCacheCollection cacheCollection;
  SDLDisplayer displayer;

  void openStream();
  void initAv();
  void read();
  void determineStream();
  // null_opt
  std::optional<ErrorDesc> openStreamComponent(AVMediaType type, uint16_t streamIndex);
public:
  explicit SDLVideoPlayer(
    const std::string& video_path,
    const std::optional<SDLVidPlayerSettings>& setting = std::nullopt
  );
  // 可能会抛出异常
  void play() override;
};

#endif //VIDEOPLAYER_H