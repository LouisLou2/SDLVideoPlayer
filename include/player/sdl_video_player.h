//
// Created by leo on 24-9-29.
//

#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <fstream>
#include <struct/packet_queue.h>
#include "util/sdl_mem.h"
#include "video_player.h"
#include "entity/play_state.h"
#include "entity/video_basic_info.h"
#include "struct/frame_queue.h"
#include "entity/player_setting.h"

#ifdef __cplusplus
extern "C" {
#include <SDL2/SDL.h>
}
#else
#include <SDL2/SDL.h>
#endif


class SDLVideoPlayer final : public VideoPlayer{

  static constexpr std::string programName = "SDLVideoPlayer";

  PlayerSettings settings;
  VideoBasicInfo videoInfo;
  PlayState playState;

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

  void setInputFormat();
  void openStream();
  void initAv();
  void initSDL();
public:
  explicit SDLVideoPlayer(
    const std::string& video_path,
    const std::optional<PlayerSettings>& setting = std::nullopt
  );
  void play() override;
};



#endif //VIDEOPLAYER_H
