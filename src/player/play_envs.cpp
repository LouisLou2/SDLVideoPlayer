//
// Created by leo on 24-10-13.
//
#include "player/play_envs.h"

#include "entity/error_desc.h"
#ifdef __cplusplus
extern "C" {
#include<SDL2/SDL.h>
}
#else
#include<SDL2/SDL.h>
#endif
bool PlayEnvs::screenInfoSet = false;
std::vector<ScreenInfo> PlayEnvs::screenInfos = {};

void PlayEnvs::ensureScreenInfoSet() {
  if (screenInfoSet) return;
  // init sdl
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw ErrorDesc::from(ExceptionType::SDLInitFailed, SDL_GetError());
  }
  uint32_t numDisplays = SDL_GetNumVideoDisplays();
  if (numDisplays < 1) {
    throw ErrorDesc::from(ExceptionType::UnsupportedHardware, "No display found By SDL");
  }
  if (numDisplays >= std::numeric_limits<uint16_t>::max()) {
    throw ErrorDesc::from(ExceptionType::UnsupportedHardware, "Too many displays, maybe there is something wrong");
  }
  SDL_DisplayMode mode;
  for (uint16_t i = 0; i < numDisplays; ++i) {
    if (SDL_GetCurrentDisplayMode(i, &mode) == 0) {
      screenInfos.emplace_back(i, mode.w, mode.h, mode.refresh_rate);
    }
  }
  if (screenInfos.empty()) {
    throw ErrorDesc::from(ExceptionType::UnsupportedHardware, "Can't get any information of displays");
  }
  screenInfoSet = true;
}