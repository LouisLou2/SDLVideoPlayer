//
// Created by leo on 24-9-29.
//

#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <fstream>
#include "player/sdl_video_player/sdl_thread_coordinator.h"
#include "struct/packet_queue.h"
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

  std::unique_ptr<SDL_Window, SDL_WindowDeleter> window;
  std::unique_ptr<SDL_Renderer, SDL_RendererDeleter> renderer;
  SDL_RendererInfo rendererInfo;

  std::ifstream videoInFile;

  PacketQueue vPktq;
  PacketQueue aPktq;
  PacketQueue sPktq;
  FrameQueue vFrameq;
  FrameQueue aFrameq;
  FrameQueue sFrameq;

  void openStream();
  void initAv();
  void initSDL();
  void read();
  void determineStream();

public:
  explicit SDLVideoPlayer(
    const std::string& video_path,
    const std::optional<SDLVidPlayerSettings>& setting = std::nullopt
  );
  void play() override;
};

#endif //VIDEOPLAYER_H