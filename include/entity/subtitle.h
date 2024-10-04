//
// Created by leo on 24-10-4.
//

#ifndef SUBTITLE_H
#define SUBTITLE_H
#include <cstdint>

#include "subtitle_rect.h"

struct Subtitle {
  uint16_t format;
  uint16_t startDisplayTime;
  uint32_t endDisplayTime;
  uint32_t numRect;
  SubtitleRect** rects;
  uint64_t pts;
};

#endif //SUBTITLE_H
