//
// Created by leo on 24-10-13.
//

#ifndef SDL_DISPLAYER_H
#define SDL_DISPLAYER_H

#ifdef __cplusplus
extern "C" {
#include <SDL2/SDL.h>
}
#else
#include <SDL2/SDL.h>
#endif
#include <memory>
#include <optional>
#include <functional>
#include "entity/error_desc.h"
#include "util/mem/sdl_mem.h"
#include "sdl_vid_player_setting.h"

class SDLDisplayer {
  // static
  static constexpr std::array<uint8_t,4> chNumAlternatives = {1,2,4,6};
  static constexpr std::array<uint32_t,5> srAlternatives = {8000, 44100, 48000, 96000, 192000};
  static constexpr uint16_t minSamplesContainedIn1Call = 512;
  static constexpr uint16_t maxCallsPerSec = 30;

  // non-static
  SDL_AudioDeviceID audioDeviceId;
  std::unique_ptr<SDL_Window, SDL_WindowDeleter> window;
  std::unique_ptr<SDL_Renderer, SDL_RendererDeleter> renderer;
  SDL_RendererInfo rendererInfo;
public:
  // void set(
  //     std::unique_ptr<SDL_Window, SDL_WindowDeleter>&& window,
  //     std::unique_ptr<SDL_Renderer, SDL_RendererDeleter>&& renderer,
  //     const SDL_RendererInfo& rendererInfo
  //   );
  std::optional<ErrorDesc> initDisplayer(const SDLVidPlayerSettings& settings);
  // 以下函数可能会更改wantedChLayOut,并且可能抛出异常
  std::optional<ErrorDesc> configAudioDisplay(
    SDL_AudioCallback callback,
    AVChannelLayout* wantedChLayOut,
    uint32_t wantedSR
  );
};

// inline void SDLDisplayer::set(
//       std::unique_ptr<SDL_Window, SDL_WindowDeleter>&& window,
//       std::unique_ptr<SDL_Renderer, SDL_RendererDeleter>&& renderer,
//       const SDL_RendererInfo& rendererInfo) {
//   assert(window && renderer);
//   this->window = std::move(window);
//   this->renderer = std::move(renderer);
//   this->rendererInfo = rendererInfo;
// }

#endif //SDL_DISPLAYER_H
