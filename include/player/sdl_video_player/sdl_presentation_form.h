//
// Created by leo on 24-10-16.
//

#ifndef SDL_PRESENTATION_FORM_H
#define SDL_PRESENTATION_FORM_H
#include <optional>

#include "const/show_mode_enum.h"

class MediaPresentForm {
  static constexpr AudioVisualForm defaultForm = AudioVisualForm::Waves;
  bool autoDetect;

  bool enableAud;
  bool enableVid;
  bool enableSub;
  AudioVisualForm audVisForm;

public:
  // MediaPresentForm构造时候仅仅代表用户意愿（所以auto就代表用户接受一切安排，all True）
  explicit MediaPresentForm(std::optional<ShowModeEnum> showModeOpt);
  //getter
  [[nodiscard]] bool isEnableAud() const { return enableAud; }
  [[nodiscard]] bool isEnableVid() const { return enableVid; }
  [[nodiscard]] bool isEnableSub() const { return enableSub; }
  [[nodiscard]] AudioVisualForm getAudVisForm() const { return audVisForm; }
  // setter
  void setForm(bool enableAud, bool enableVid, bool enableSub, AudioVisualForm audVisForm = defaultForm);
  // void setForm(bool enableAud, bool enableVid, bool enableSub);
  [[nodiscard]] inline std::optional<AudioVisualForm> getAudioVisualFormIfOnlyAud() const;
  [[nodiscard]] inline bool onlyAud() const;
};

inline std::optional<AudioVisualForm> MediaPresentForm::getAudioVisualFormIfOnlyAud() const {
  return onlyAud() ? std::optional(audVisForm) : std::nullopt;
}

inline bool MediaPresentForm::onlyAud() const {
  // 只做一个判断即可，因为图画不显示，那么字幕也不会显示，但是不可能都不显示，所以音频一定显示
  return !enableVid;
}

#endif //SDL_PRESENTATION_FORM_H
