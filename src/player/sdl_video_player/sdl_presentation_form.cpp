//
// Created by leo on 24-10-16.
//
#include "player/sdl_video_player/sdl_presentation_form.h"

#include <cstdint>
#include <cassert>

MediaPresentForm::MediaPresentForm(ShowModeEnum showMode):
  enableAud(static_cast<uint8_t>(showMode) & 0b00010000),
  enableVid(static_cast<uint8_t>(showMode) & 0b00001000),
  enableSub(static_cast<uint8_t>(showMode) & 0b00000100),
  audVisForm(static_cast<AudioVisualForm>(static_cast<uint8_t>(showMode) & 0b00000011)) {
  assert(enableAud || enableVid || enableSub); // 不可能三者都不显示
  assert(enableVid || !enableVid&&!enableSub ); // 字幕不能脱离图像显示
}
