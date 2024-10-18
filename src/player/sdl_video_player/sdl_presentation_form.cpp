//
// Created by leo on 24-10-16.
//
#include "player/sdl_video_player/sdl_presentation_form.h"

#include <cstdint>
#include <cassert>

MediaPresentForm::MediaPresentForm(ShowModeEnum showMode):
  autoDetect(showMode == ShowModeEnum::Auto), // 如果是Auto，那么就是自动检测，下面就没必要位运算了
  enableAud(autoDetect ? true : static_cast<uint8_t>(showMode) & 0b00010000),
  enableVid(autoDetect ? true : static_cast<uint8_t>(showMode) & 0b00001000),
  enableSub(autoDetect ? true : static_cast<uint8_t>(showMode) & 0b00000100),
  audVisForm(autoDetect ? defaultForm : static_cast<AudioVisualForm>(static_cast<uint8_t>(showMode) & 0b00000011)) {
  if (showMode != ShowModeEnum::Auto) {
    assert(enableAud || enableVid || enableSub); // 不可能三者都不显示
    assert(enableVid || !enableVid&&!enableSub ); // 字幕不能脱离图像显示
  }
}

void MediaPresentForm::setForm(bool enableAud, bool enableVid, bool enableSub, AudioVisualForm audVisForm) {
  assert(enableAud || enableVid || enableSub); // 不可能三者都不显示
  assert(enableVid || !enableVid&&!enableSub ); // 字幕不能脱离图像显示
  this->enableAud = enableAud;
  this->enableVid = enableVid;
  this->enableSub = enableSub;
}