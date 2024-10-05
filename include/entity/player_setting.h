//
// Created by leo on 24-10-4.
//

#ifndef PLAYER_SETTING_H
#define PLAYER_SETTING_H
#include <cstdint>
#include <optional>
#include <string>

#include "const/input_format_enum.h"

#define DEF_WIN_WIDTH 640
#define DEF_WIN_HEIGHT 480

struct PlayerSettings {
  friend class SDLVideoPlayer;
private:
  std::string windowTitle;
  bool enableNetwork;
  bool disableAud;
  bool disableVid;
  bool alwaysOnTop;
  bool borderless;
  uint32_t windowWidth;
  uint32_t windowHeight;

  bool specifiedInputFormat;
  InputFormatEnum inputFormat;

public:
  PlayerSettings(
    auto&& title = "Unnamed Player",
    bool enableNetwork = true,
    bool disableAud = false,
    bool disableVideo = false,
    bool alwaysOnTop = false,
    bool borderless = false,
    uint32_t windowWidth = DEF_WIN_WIDTH,
    uint32_t windowHeight = DEF_WIN_HEIGHT,
    std::optional<InputFormatEnum> inputFormat = std::nullopt
  );
  // 拷贝构造函数
  PlayerSettings(const PlayerSettings& setting) = default;
  // 移动构造函数
  /*
   * 为什么这里要声明移动构造函数？
   * 当传递一个右值引用的时候，我们的PlayerSettings构造函数第一个形参是universial reference, 并且剩下的形参都是有默认值的，
   * 所以这个构造函数可以接受右值引用，可能会调用成构造函数，这不是预期的
   */
  PlayerSettings(PlayerSettings&& setting) = default;
};


PlayerSettings::PlayerSettings(
  auto&& title,
  bool enableNetwork,
  bool disableAud,
  bool disableVideo,
  bool alwaysOnTop,
  bool borderless,
  uint32_t windowWidth,
  uint32_t windowHeight,
  std::optional<InputFormatEnum> inputFormat
) :
  windowTitle(std::forward<decltype(title)>(title)),
  enableNetwork(enableNetwork),
  disableAud(disableAud),
  disableVid(disableVideo),
  alwaysOnTop(alwaysOnTop),
  borderless(borderless),
  windowWidth(windowWidth),
  windowHeight(windowHeight),
  specifiedInputFormat(inputFormat.has_value()){
  if (specifiedInputFormat) this->inputFormat = inputFormat.value(); // 如果没有输入格式，
}

#endif //PLAYER_SETTING_H
