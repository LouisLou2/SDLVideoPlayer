//
// Created by leo on 24-10-13.
//

#ifndef SDL_DISPLAYER_H
#define SDL_DISPLAYER_H
#include "entity/audio_params.h"

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
  static constexpr uint16_t wantedCallsPerSec = 24;
  static constexpr uint16_t maxCallsPerSec = 35;

  // non-static
  SDL_AudioDeviceID audioDeviceId;
  std::unique_ptr<SDL_Window, SDL_WindowDeleter> window;
  std::unique_ptr<SDL_Renderer, SDL_RendererDeleter> renderer;
  SDL_RendererInfo rendererInfo;
public:
  std::optional<ErrorDesc> initDisplayer(const SDLVidPlayerSettings& settings);

  static AudioParams copySDLAudioSpecToAudioParams(
    const SDL_AudioSpec& spec,
    const AVChannelLayout* ch_layout) noexcept(false);

  // 以下函数可能会更改wantedChLayOut,并且可能抛出异常
  AudioParams configAudioDisplay(
    SDL_AudioCallback callback,
    AVChannelLayout* wantedChLayOut,
    uint32_t wantedSR,
    SDL_AudioFormat mustBeFormat
  ) noexcept(false);
};

#endif //SDL_DISPLAYER_H
