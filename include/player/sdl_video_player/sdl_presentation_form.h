//
// Created by leo on 24-10-16.
//

#ifndef SDL_PRESENTATION_FORM_H
#define SDL_PRESENTATION_FORM_H
#include "const/show_mode_enum.h"

class MediaPresentForm {
  bool enableAud;
  bool enableVid;
  bool enableSub;

  AudioVisualForm audVisForm;

public:
  explicit MediaPresentForm(ShowModeEnum showMode);
  //getter
  [[nodiscard]] bool isEnableAud() const { return enableAud; }
  [[nodiscard]] bool isEnableVid() const { return enableVid; }
  [[nodiscard]] bool isEnableSub() const { return enableSub; }
  [[nodiscard]] AudioVisualForm getAudVisForm() const { return audVisForm; }
};

#endif //SDL_PRESENTATION_FORM_H
