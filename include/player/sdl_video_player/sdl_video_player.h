//
// Created by leo on 24-9-29.
//

#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "sdl_cache_collection.h"
#include "sdl_media_filter_info.h"
#include "player/sdl_video_player/sdl_thread_coordinator.h"
#include "util/mem/sdl_mem.h"
#include "player/video_player.h"
#include "player/sdl_video_player/sdl_play_state.h"
#include "player/sdl_video_player/sdl_video_basic_info.h"
#include "struct/frame_queue.h"
#include "player/sdl_video_player/sdl_vid_player_setting.h"

#ifdef __cplusplus
extern "C" {
#include <SDL2/SDL.h>
}
#else
#include <SDL2/SDL.h>
#endif


class SDLVideoPlayer final : public VideoPlayer{

  static constexpr std::string programName = "SDLVideoPlayer";

  SDLVidPlayerSettings settings;
  VideoBasicInfo videoInfo;
  SDLPlayState playState;
  SDLThreadCoordinator coordinator;
  SDLMediaFilterInfo mediaFilterInfo;
  SDLPlayerCacheCollection cacheCollection;

  std::unique_ptr<SDL_Window, SDL_WindowDeleter> window;
  std::unique_ptr<SDL_Renderer, SDL_RendererDeleter> renderer;
  SDL_RendererInfo rendererInfo;

  void openStream();
  void initAv();
  void initSDL();
  void initHardWareAfterSDL();
  void read();
  void determineStream();
  // null_opt
  std::optional<ErrorDesc> openStreamComponent(AVMediaType type, uint16_t streamIndex);
  std::optional<ErrorDesc> configAudioFilter();
public:
  explicit SDLVideoPlayer(
    const std::string& video_path,
    const std::optional<SDLVidPlayerSettings>& setting = std::nullopt
  );
  void play() override;
};

#endif //VIDEOPLAYER_H