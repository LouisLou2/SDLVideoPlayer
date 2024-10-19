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
#include "const/play_sync_type.h"
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

  static constexpr double maxPlaySpeed = 5.0;
  static constexpr double minPlaySpeed = 0.25;
  static constexpr double defPlaySpeed = 1.0;
private:
  // static
  static constexpr uint8_t maxVolume = 100;
  static constexpr uint8_t minVolume = 0;
  static constexpr uint8_t defVolume = maxVolume/2;
  std::string windowTitle;
  bool enableNetwork;
  double playSpeed;

  /*此值仅仅是为了构造playState中的MediaPresentForm presentForm,
   *此变量初始化后会在联系实际的时候再用一次，之后就不会访问了，
   *但是这个值是不需要改变了，因为之后都由playState中的presentForm来表示，这个字段失去价值
   */
  const std::optional<ShowModeEnum> showMode;

  const std::optional<PlaySyncType> syncType;

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
  uint8_t decoderThreadsNum; // 解码线程数, 0表示自动
  uint8_t filterThreadsNum; // 过滤线程数, 0表示自动

  // TODO：关于audioFilterGraphStr我自己也还不是太明白，后续再研究
  std::string audioFilterGraphStr;
  std::map<std::string, std::string> swrOpts;
public:
  explicit SDLVidPlayerSettings(
    auto&& title = "Unnamed Player",
    bool enableNetwork = true,
    std::optional<double> playSpeed = std::nullopt,
    std::optional<ShowModeEnum> showMode = std::nullopt,
    std::optional<PlaySyncType> syncType = std::nullopt,
    uint16_t useDisplayIndex = 0,
    bool alwaysOnTop = false,
    bool borderless = false,
    uint32_t windowWidth =defWinWidth,
    uint32_t windowHeight = defWinHeight,
    std::optional<InputFormatEnum> inputFormat = std::nullopt,
    std::optional<uint8_t> volumeWithin100 = std::nullopt,
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
  inline void checkAndSetSpeed(std::optional<double> speedOpt);
  inline void checkAndSetVolume(std::optional<uint8_t> volumeWithin100Opt);
  inline void checkDecoderThreadsNum(std::optional<uint8_t> threadsNumOpt);
  inline void checkFilterThreadsNum(std::optional<uint8_t> threadsNumOpt);

  // 注意此函数只会检查而不会调整，所以参数列表中的std::opt其实也能代表完整的意义，因为并没有进行适应与调整
  inline void checkSetShowModeSyncType() const;
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
  std::optional<double> playSpeed,
  std::optional<ShowModeEnum> showMode,// 注意这个的现在，代表用户的期望，不一定能满足，因为就像用户期待音频+视频，但是实际上只有音频，所以后续一定会修正
  std::optional<PlaySyncType> syncType,
  uint16_t useDisplayIndex,
  bool alwaysOnTop,
  bool borderless,
  uint32_t windowWidth,
  uint32_t windowHeight,
  std::optional<InputFormatEnum> inputFormat,
  std::optional<uint8_t> volumeWithin100,
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
  showMode(showMode),
  syncType(syncType),
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
  audioFilterGraphStr(std::move(audioFilterGraphStr)),
  swrOpts(std::move(swrOpts)) {
  // 关于showMode,其实如果showMode是不播放图像的，那么例如windowWidth和windowHeight就没有意义了，其实可以检查，这里就不检查了
  if (specifiedInputFormat) this->inputFormat = inputFormat.value(); // 如果指定输入格式，
  if (specifiedSeekType) this->seekType = seekType.value();

  checkAndSetVolume( volumeWithin100 );
  checkAndSetSpeed( playSpeed );
  checkDecoderThreadsNum( decodeThreadsNum );
  checkFilterThreadsNum( filterThreadsNum );
  // 此函数已经在就是检查已经复制完毕的成员变量，所以不用传参
  checkSetShowModeSyncType();
}

inline void SDLVidPlayerSettings::checkAndSetVolume(std::optional<uint8_t> volumeWithin100Opt) {
  if (!volumeWithin100Opt.has_value()) {
    this->volumeWithin100 = defVolume;
    return;
  }
  uint8_t volumeWithin100 = volumeWithin100Opt.value();
  if (volumeWithin100 <= maxVolume && volumeWithin100 >= minVolume) {
    this->volumeWithin100 = volumeWithin100;
    return;
  }
  // 修正
  this->volumeWithin100 = std::clamp(volumeWithin100, minVolume, maxVolume);
  // 给出警告
  PlayerLogger::log(ErrorDesc::from(
    ExceptionType::AutoAdjust,
    std::format("Volume should be in range [{}, {}], but got {}, so adjust to {}", minVolume, maxVolume, volumeWithin100, this->volumeWithin100)
    )
  );
}

inline void SDLVidPlayerSettings::checkAndSetSpeed(std::optional<double> speedOpt) {
  if (!speedOpt.has_value()) {
    this->playSpeed = defPlaySpeed;
    return;
  }
  // 一旦指定了播放速度，那么就要检查是否在合理范围内
  double speed = speedOpt.value();
  if (speed <= maxPlaySpeed && speed >= minPlaySpeed) {
    this->playSpeed = speed;
    return;
  }
  // 修正
  this->playSpeed = std::clamp(speed, minPlaySpeed, maxPlaySpeed);
  // 给出警告
  PlayerLogger::log(ErrorDesc::from(
    ExceptionType::AutoAdjust,
    std::format("Play speed should be in range [{}, {}], but got {}, so adjust to {}", minPlaySpeed, maxPlaySpeed, speed, this->playSpeed)
    )
  );
  // 关于硬件的冲突检查，这里不做(在)，因为如果这里抛出异常，那么在构造函数中就会抛出异常，虽然仍然可以释放资源，但是对于VideoPlayer的构造来说，setting通常是在初始化列表，不太好try-catch
}

inline void SDLVidPlayerSettings::checkDecoderThreadsNum(std::optional<uint8_t> threadsNumOpt) {
  if (!threadsNumOpt.has_value()) {
    this->decoderThreadsNum = threadsNumAuto;
    return;
  }
  uint8_t threadsNum = threadsNumOpt.value();
  if (threadsNum != 0 && threadsNum <= 16) {
    this->decoderThreadsNum = threadsNum;
    return;
  }
  // 给出警告
  this->decoderThreadsNum = threadsNumAuto;
  PlayerLogger::log(ErrorDesc::from(
    ExceptionType::AutoAdjust,
    std::format("decode threads num should be in range [{}, {}], but got {}, so adjust to auto", minDecoderThreads, maxDecoderThreads, threadsNum)
    )
  );
}

inline void SDLVidPlayerSettings::checkFilterThreadsNum(std::optional<uint8_t> threadsNumOpt) {
  if (!threadsNumOpt.has_value()) {
    this->filterThreadsNum = threadsNumAuto;
    return;
  }
  uint8_t threadsNum = threadsNumOpt.value();
  if (threadsNum != 0 && threadsNum <= 16) {
    this->filterThreadsNum = threadsNum;
    return;
  }
  // 给出警告
  this->filterThreadsNum = threadsNumAuto;
  PlayerLogger::log(ErrorDesc::from(
    ExceptionType::AutoAdjust,
    std::format("filter threads num should be in range [{}, {}], but got {}, so adjust to auto", minFilterThreads, maxFilterThreads, threadsNum)
    )
  );
}

inline void SDLVidPlayerSettings::checkSetShowModeSyncType() const {
  if (showMode && showMode != ShowModeEnum::All && syncType && syncType != PlaySyncType::ExternalMaster) {
    // 如果没有指定showMode，那么具体怎么播就有赖于实际的流了，这里还不能确定，所以没有理由抛出异常，同理，我们不知道真实情况会不会无法满足specifiedSyncType,所以也不抛异常
    ShowModeEnum showMode = this->showMode.value();
    PlaySyncType syncType = this->syncType.value();
    // 到此说明用户既指定了showMode，又指定了syncType
    if (syncType == PlaySyncType::AudioMaster && !ShowModeUtil::isEnableAudio(showMode)) {
      // 如果用户指定了音频为主，但是showMode中没有音频，那么就抛出异常
      throw ErrorDesc::from(ExceptionType::SettingConflict, "AudioMaster sync type must have audio stream");
    }
    if (syncType == PlaySyncType::VideoMaster && !ShowModeUtil::isEnableVideo(showMode)) {
      // 如果用户指定了视频为主，但是showMode中没有视频，那么就抛出异常
      throw ErrorDesc::from(ExceptionType::SettingConflict, "VideoMaster sync type must have video stream");
    }
  }
}
#endif //SDL_VIDEO_PLAYER_SETTING_H
