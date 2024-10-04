//
// Created by leo on 24-10-4.
//

#ifndef PLAYER_SETTING_H
#define PLAYER_SETTING_H
#include <cstdint>
#include <string>

struct PlayerSettings {
  std::string windowTitle;
  bool enableNetwork;
  bool disableAud;
  bool disableVid;
  bool alwaysOnTop;
  bool borderless;
  uint32_t windowWidth;
  uint32_t windowHeight;

  PlayerSettings(
    auto&& title,
    bool enableNetwork,
    bool disableAud,
    bool disableVideo,
    bool alwaysOnTop,
    bool borderless,
    uint32_t windowWidth,
    uint32_t windowHeight
  );
  // 拷贝构造函数
  PlayerSettings(const PlayerSettings& setting) = default;
};


PlayerSettings::PlayerSettings(
  auto&& title,
  bool enableNetwork,
  bool disableAud,
  bool disableVideo,
  bool alwaysOnTop,
  bool borderless,
  uint32_t windowWidth,
  uint32_t windowHeight
) : windowTitle(std::forward<decltype(title)>(title)),
  enableNetwork(enableNetwork),
  disableAud(disableAud),
  disableVid(disableVideo),
  alwaysOnTop(alwaysOnTop),
  borderless(borderless),
  windowWidth(windowWidth),
  windowHeight(windowHeight) {}

#endif //PLAYER_SETTING_H
