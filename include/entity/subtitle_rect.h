//
// Created by leo on 24-9-30.
//

#ifndef SUBTITLE_RECT_H
#define SUBTITLE_RECT_H
#include <array>
#include <cstdint>
#include <string_view>
#include "const/subtitle_type.h"

struct SubtitleRect {
  int32_t x{};
  int32_t y{};
  int32_t w{};
  int32_t h{};
  uint16_t nb_colors{};
  std::array<std::string_view,4> datas;
  std::array<uint32_t,4> linesize{};
  SubtitleTypeEnum type{};
  char* text{};
  char* ass{};
  int flags{};
};



#endif //SUBTITLE_RECT_H
