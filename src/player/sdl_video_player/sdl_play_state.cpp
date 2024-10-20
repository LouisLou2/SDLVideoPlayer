//
// Created by leo on 24-10-4.
//

#include "player/sdl_video_player/sdl_play_state.h"
#ifdef __cplusplus
extern "C" {
#include <SDL2/SDL_audio.h>
}
#else
#include <SDL2/SDL_audio.h>
#endif

// 此函数遵循AudioMaster优先的原则
std::optional<ErrorDesc> SDLPlayState::correctPresentForm(bool hasAud, bool hasVid, bool hasSub, std::optional<PlaySyncType> originalSyncType) {
  if (autoShowMode) {
    // 这里说明用户一开始选择的就是Auto，那么就根据实际情况来决定
    if (!hasAud && !hasVid) {
      // 用户要求不显示的一定不显示，用户要求显示的尽量满足，但是如果结合用户要求和实际情况，导致既不能播放音频也不能播放视频，那么就报错
      return ErrorDesc::from(ExceptionType::NoAudioAndVideo, "No audio and video stream to play according to your setting and real media file");
    }
  }else {
    // 说明用户显示指定了显示什么，不显示什么，那么如果有和用户指定的相违背的，就报错，也不做任何改动
    // 而且，用户一开始的期望理论上是合适的（不合适的不允准设置），所以这里不再检测是否合适，而只检测是否实际与用户期望相违背
    if (!hasAud && presentForm.isEnableAud()) {
      return ErrorDesc::from(ExceptionType::WantedStreamNotFound, "Audio stream is not shown according to your setting");
    }
    if (!hasVid && presentForm.isEnableVid()) {
      return ErrorDesc::from(ExceptionType::WantedStreamNotFound, "Video stream is not shown according to your setting");
    }
    if (!hasSub && presentForm.isEnableSub()) {
      return ErrorDesc::from(ExceptionType::WantedStreamNotFound, "Subtitle stream is not shown according to your setting");
    }
  }
  // 这里说明showMode正常
  presentForm.setForm(hasAud, hasVid, hasSub);
  // 开始处理syncType
  if (!originalSyncType.has_value()) {
    // 说明用户没有指定同步类型，那么就根据实际情况来决定
    if (presentForm.isEnableAud()) clkGroup.setMasterClock(PlaySyncType::AudioMaster);
    else clkGroup.setMasterClock(PlaySyncType::VideoMaster); // 前面约束了，所以这里不可能出现两者都没有的情况
  }else {
    if (originalSyncType.value() == PlaySyncType::AudioMaster) {
      if (!presentForm.isEnableAud()) {
        return ErrorDesc::from(ExceptionType::WantedStreamNotFound, "Audio stream is not shown according to your setting");
      }
      clkGroup.setMasterClock(PlaySyncType::AudioMaster);
    }else if (originalSyncType.value() == PlaySyncType::VideoMaster) {
      if (!presentForm.isEnableVid()) {
        return ErrorDesc::from(ExceptionType::WantedStreamNotFound, "Video stream is not shown according to your setting");
      }
      clkGroup.setMasterClock(PlaySyncType::VideoMaster);
    }else {
      clkGroup.setMasterClock(PlaySyncType::ExternalMaster);
    }
  }
  return std::nullopt;
}

SDLPlayState::SDLPlayState(
std::optional<ShowModeEnum> showModeOpt,
  double playSpeed,
  const uint32_t& audqSerialRef,
  const uint32_t& vidqSerialRef,
  const uint32_t& extqSerialRef
  ):
  serial(0),
  lastVidStInd(-1),
  lastAudStInd(-1),
  lastSubStInd(-1),
  vidStInd(-1),
  audStInd(-1),
  subStInd(-1),
  // 以上三个index是播放的流的index，-1表示没有播放
  width(0),
  height(0),
  x(0),
  y(0),
  // 窗口的位置和大小，都预设为0
  mute(false),
  paused(false),
  volume(SDL_MIX_MAXVOLUME / 2),
  clkGroup(playSpeed, this->paused, audqSerialRef, vidqSerialRef, extqSerialRef),
  audioClockSerial(-1),
  eof(false), // 默认没有到达文件尾
  abortReq(false), // 默认没有退出请求
  maxFrameDuration(0),// 0是非法的，这个也必须后来改动
  playOffset(0),
  defaultPicWidth(0),
  defaultPicHeight(0),
  autoShowMode(!showModeOpt.has_value()),
  presentForm(showModeOpt){
  // clkGroup(playSpeed)其中的playSpeed一定是合法的，会提前检查
}
