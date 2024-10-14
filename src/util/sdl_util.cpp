//
// Created by leo on 24-10-15.
//
#include "util/sdl_util.h"

std::optional<AVSampleFormat> SDLUtil::getCorAVFormat(SDL_AudioFormat sdlAudFmt) {
  for (const auto& [avFmt, sdlFmt] : formatPairList) {
    if (sdlFmt == sdlAudFmt) {
      return avFmt;
    }
  }
  return std::nullopt;
}
