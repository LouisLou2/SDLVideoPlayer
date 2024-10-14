//
// Created by leo on 24-10-4.
//

#ifndef SDL_VIDEO_PLAYER_SETTING_H
#define SDL_VIDEO_PLAYER_SETTING_H

#include <map>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <format>
#include <bits/algorithmfwd.h>
#include "const/show_mode_enum.h"
#ifdef __cplusplus
extern "C"{
#include <libavutil/dict.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <SDL2/SDL_audio.h>
}
#else
#include <libavutil/dict.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <SDL2/SDL_audio.h>
#endif

#include "util/logger/player_logger.h"
#include "player/player_setting.h"
#include "const/input_format_enum.h"
#include "const/seek_type.h"

class SDLVidPlayerSettings : public PlayerSetting{
  friend class SDLVideoPlayer;
  friend class SDLDisplayer;
public:
  static constexpr AVSampleFormat theOnlyAllowedSRFmt_FF = AV_SAMPLE_FMT_S16;
  static constexpr SDL_AudioFormat theOnlyAllowedSRFmt_SDL = AUDIO_S16SYS;
  static constexpr uint32_t defWinWidth = 640;
  static constexpr uint32_t defWinHeight = 480;
  static constexpr uint8_t maxDecoderThreads = 16;
  static constexpr uint8_t minDecoderThreads = 1;
  static constexpr uint8_t maxFilterThreads = 16;
  static constexpr uint8_t minFilterThreads = 1;
  static constexpr uint8_t threadsNumAuto = 0;
private:
  // static
  static constexpr uint8_t maxVolume = 100;
  static constexpr uint8_t minVolume = 0;
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
  // 5个，分别是视频，音频，字幕，数据，附加数据的流规格筛选规则
  std::array<const char*, AVMEDIA_TYPE_NB> wantedStreamSpec;
  // 允许在decoder找不到指定的流时，使用默认的流
  bool forceSpecifiedDecoder;
  uint8_t lowres; // 低分辨率等级，0表示不使用低分辨率
  bool decodeFastMode;
  uint8_t decoderThreadsNum; // 解码线程数
  uint8_t filterThreadsNum; // 过滤线程数

  // TODO：关于audioFilterGraphStr我自己也还不是太明白，后续再研究
  std::string audioFilterGraphStr;
  std::map<std::string, std::string> swrOpts;
public:
  explicit SDLVidPlayerSettings(
    auto&& title = "Unnamed Player",
    bool enableNetwork = true,
    bool disableAud = false,
    bool disableVideo = false,
    bool disableSub = false,
    ShowModeEnum showMode = ShowModeEnum::Video, // 默认显示视频,即全部显示
    uint16_t useDisplayIndex = 0,
    bool alwaysOnTop = false,
    bool borderless = false,
    uint32_t windowWidth =defWinWidth,
    uint32_t windowHeight = defWinHeight,
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
    bool forceSpecifiedDecoder = false,
    uint8_t lowres = 0,
    bool decodeFastMode = false,
    std::optional<uint8_t>decodeThreadsNum = std::nullopt,
    std::optional<uint8_t> filterThreadsNum = std::nullopt,

    std::string audioFilterGraphStr = "",
    // TODO: 配置上使用map是偷懒的行为，要不就提取类进行静态的配置， 要不就必须对传入的参数进行检查， 这个之后再考虑吧
    std::map<std::string, std::string>&& swrOpts = {}
  );
  // 拷贝构造函数
  SDLVidPlayerSettings(const SDLVidPlayerSettings& setting) = default;
  // 移动构造函数
  /*
   * 为什么这里要声明移动构造函数？
   * 当传递一个右值引用的时候，我们的PlayerSettings构造函数第一个形参是universal reference, 并且剩下的形参都是有默认值的，
   * 所以这个构造函数可以接受右值引用，可能会调用成构造函数，这不是预期的，可以将
   */
  SDLVidPlayerSettings(SDLVidPlayerSettings&& setting) = default;

  [[nodiscard]] std::optional<ErrorDesc> checkConflictsWithHardware() const;
  [[nodiscard]] AVDictionary* getFormatOpt() const;
  [[nodiscard]] std::vector<AVDictionary*> getCodecOpts(const AVFormatContext* fmtCtx) const;
  // TODO: 此函数拿到的信息太丰富了，后续尝试精简
  [[nodiscard]] AVDictionary* filterOpts(const AVCodec* codec, const AVCodecContext* codecCtx, const AVFormatContext* fmt, const AVStream* st);
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
  bool forceSpecifiedDecoder,
  uint8_t lowres,
  bool decodeFastMode,
  std::optional<uint8_t> decodeThreadsNum,
  std::optional<uint8_t> filterThreadsNum,
  std::string audioFilterGraphStr,
  std::map<std::string, std::string>&& swrOpts
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
  forceSpecifiedDecoder(forceSpecifiedDecoder),
  lowres(lowres),
  decodeFastMode(decodeFastMode),
  decoderThreadsNum(decodeThreadsNum.value_or(0)),
  filterThreadsNum(filterThreadsNum.value_or(0)),
  audioFilterGraphStr(std::move(audioFilterGraphStr)),
  swrOpts(std::move(swrOpts))
{
  if (specifiedInputFormat) this->inputFormat = inputFormat.value(); // 如果没有输入格式，
  if (specifiedSeekType) this->seekType = seekType.value();
  if (volumeWithin100 > maxVolume || volumeWithin100 < minVolume) {
    // 修正
    this->volumeWithin100 = std::clamp(volumeWithin100, minVolume, maxVolume);
    // 给出警告
    PlayerLogger::log(ErrorDesc::from(
      ExceptionType::AutoAdjust,
      std::format("Volume should be in range [{}, {}], but got {}, so adjust to {}", minVolume, maxVolume, volumeWithin100, this->volumeWithin100)
      )
    );
  }
  if (decodeThreadsNum.has_value() && (decodeThreadsNum == 0 || decodeThreadsNum>16) ) {
    // 给出警告
    this->decoderThreadsNum = threadsNumAuto;
    PlayerLogger::log(ErrorDesc::from(
      ExceptionType::AutoAdjust,
      std::format("decode threads num should be in range [{}, {}], but got {}, so adjust to auto", minDecoderThreads, maxDecoderThreads, decodeThreadsNum.value())
      )
    );
  }
  if (filterThreadsNum.has_value() && (filterThreadsNum == 0 || filterThreadsNum>16) ) {
    // 给出警告
    this->filterThreadsNum = threadsNumAuto;
    PlayerLogger::log(ErrorDesc::from(
      ExceptionType::AutoAdjust,
      std::format("filter threads num should be in range [{}, {}], but got {}, so adjust to auto", minFilterThreads, maxFilterThreads, filterThreadsNum.value())
      )
    );
  }
  // 关于硬件的冲突检查，这里不做(在)，因为如果这里抛出异常，那么在构造函数中就会抛出异常，虽然仍然可以释放资源，但是对于VideoPlayer的构造来说，setting通常是在初始化列表，不太好try-catch
}

#endif //SDL_VIDEO_PLAYER_SETTING_H
