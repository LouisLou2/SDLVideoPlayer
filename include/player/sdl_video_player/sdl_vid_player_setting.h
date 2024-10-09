//
// Created by leo on 24-10-4.
//

#ifndef SDL_VIDEO_PLAYER_SETTING_H
#define SDL_VIDEO_PLAYER_SETTING_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "const/show_mode_enum.h"
#ifdef __cplusplus
extern "C"{
#include <libavutil/dict.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}
#else
#include <libavutil/dict.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#endif

#include "util/logger/player_logger.h"
#include "player/player_setting.h"
#include "const/input_format_enum.h"
#include "const/seek_type.h"

#define DEF_WIN_WIDTH 640
#define DEF_WIN_HEIGHT 480

#define MAX_VOLUME 100
#define MIN_VOLUME 0

class SDLVidPlayerSettings : public PlayerSetting{
  friend class SDLVideoPlayer;

  std::string windowTitle;
  bool enableNetwork;
  bool disableAud;
  bool disableVid;
  bool disableSub;
  ShowModeEnum showMode;

  uint16_t useDisplayIndex;
  bool alwaysOnTop;
  bool borderless;
  uint32_t windowWidth;
  uint32_t windowHeight;

  bool specifiedInputFormat;
  InputFormatEnum inputFormat;

  uint8_t volumeWithin100;
  bool scanAllPmtsSet;
  bool genPts;
  bool findStreamInfo;
  bool showStatus;

  bool specifiedSeekType;
  SeekType seekType;

  uint32_t playOffsetSec;

  std::string videoDecoderName;
  std::string audioDecoderName;
  std::string subDecoderName;
  // 允许在decoder找不到指定的流时，使用默认的流
  bool forceSpecifiedDecoder;

  // 5个，分别是视频，音频，字幕，数据，附加数据的流规格筛选规则
  std::array<const char*, AVMediaType::AVMEDIA_TYPE_NB> wantedStreamSpec;
public:
  explicit SDLVidPlayerSettings(
    auto&& title = "Unnamed Player",
    bool enableNetwork = true,
    bool disableAud = false,
    bool disableVideo = false,
    bool disableSub = false,
    ShowModeEnum showMode = ShowModeEnum::None,
    uint16_t useDisplayIndex = 0,
    bool alwaysOnTop = false,
    bool borderless = false,
    uint32_t windowWidth = DEF_WIN_WIDTH,
    uint32_t windowHeight = DEF_WIN_HEIGHT,
    std::optional<InputFormatEnum> inputFormat = std::nullopt,
    uint8_t volume = 40,
    bool scanAllPmtsSet = true,
    bool genPts = false,
    bool findStreamInfo = true,
    bool showStatus = false, // 默认不显示流信息
    std::optional<SeekType> seekType = std::nullopt,
    uint32_t playOffsetSec = 0,
    std::array<const char*, AVMediaType::AVMEDIA_TYPE_NB> wantedStreamSpec = {
      nullptr, nullptr, nullptr, nullptr, nullptr
    },
    std::string videoDecoderName = "",
    std::string audioDecoderName = "",
    std::string subDecoderName = "",
    bool forceSpecifiedDecoder = false
  );
  // 拷贝构造函数, 目前是用不到的
  SDLVidPlayerSettings(const SDLVidPlayerSettings& setting) = default;
  // 移动构造函数
  /*
   * 为什么这里要声明移动构造函数？
   * 当传递一个右值引用的时候，我们的PlayerSettings构造函数第一个形参是universal reference, 并且剩下的形参都是有默认值的，
   * 所以这个构造函数可以接受右值引用，可能会调用成构造函数，这不是预期的，可以将
   */
  SDLVidPlayerSettings(SDLVidPlayerSettings&& setting) = default;

  [[nodiscard]] AVDictionary* getFormatOpt() const;
  [[nodiscard]] std::vector<AVDictionary*> getCodecOpts(const AVFormatContext* fmtCtx);
};

SDLVidPlayerSettings::SDLVidPlayerSettings(
  auto&& title,
  bool enableNetwork,
  bool disableAud,
  bool disableVideo,
  bool disableSub,
  ShowModeEnum showMode,
  uint16_t useDisplayIndex,
  bool alwaysOnTop,
  bool borderless,
  uint32_t windowWidth,
  uint32_t windowHeight,
  std::optional<InputFormatEnum> inputFormat,
  uint8_t volumeWithin100,
  bool scanAllPmtsSet,
  bool genPts,
  bool findStreamInfo,
  bool showStatus,
  std::optional<SeekType> seekType,
  uint32_t playOffsetSec,
  std::array<const char*, AVMediaType::AVMEDIA_TYPE_NB> wantedStreamSpec,
  std::string videoDecoderName,
  std::string audioDecoderName,
  std::string subDecoderName,
  bool forceSpecifiedDecoder
) :
  windowTitle(std::forward<decltype(title)>(title)),
  enableNetwork(enableNetwork),
  disableAud(disableAud),
  disableVid(disableVideo),
  disableSub(disableSub),
  showMode(showMode),
  useDisplayIndex(useDisplayIndex),
  alwaysOnTop(alwaysOnTop),
  borderless(borderless),
  windowWidth(windowWidth),
  windowHeight(windowHeight),
  specifiedInputFormat(inputFormat.has_value()),
  scanAllPmtsSet(scanAllPmtsSet),
  genPts(genPts),
  findStreamInfo(findStreamInfo),
  showStatus(showStatus),
  specifiedSeekType(seekType.has_value()),
  playOffsetSec(playOffsetSec),
  wantedStreamSpec(wantedStreamSpec),
  videoDecoderName(std::move(videoDecoderName)),
  audioDecoderName(std::move(audioDecoderName)),
  subDecoderName(std::move(subDecoderName)),
  forceSpecifiedDecoder(forceSpecifiedDecoder)
{
  if (specifiedInputFormat) this->inputFormat = inputFormat.value(); // 如果没有输入格式，
  if (specifiedSeekType) this->seekType = seekType.value();
  if (volumeWithin100 > MAX_VOLUME) {
    // 给出警告
    PlayerLogger::log(ErrorDesc::from(
      ExceptionType::AutoAdjust,
      "Volume should be in range [0, 100], but got " + std::to_string(volumeWithin100)
    ));
    // 修正
    this->volumeWithin100 = MAX_VOLUME;
  }
}

#endif //SDL_VIDEO_PLAYER_SETTING_H
