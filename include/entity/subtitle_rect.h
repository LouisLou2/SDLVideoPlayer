//
// Created by leo on 24-9-30.
//

#ifndef SUBTITLE_RECT_H
#define SUBTITLE_RECT_H
#include <array>
#include <cstdint>
#include <string_view>


struct SubtitleRect {
  int32_t x;
  int32_t y;
  int32_t w;
  int32_t h;
  uint16_t nb_colors;
  std::array<std::string_view,4> datas;

};



#endif //SUBTITLE_RECT_H
