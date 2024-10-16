//
// Created by leo on 24-10-4.
//

#ifndef PLAY_STATE_H
#define PLAY_STATE_H
#include <cstdint>
#include <chrono>
#include "sdl_clock_group.h"
#ifdef __cplusplus
extern "C" {
#include <SDL2/SDL_audio.h>
}
#else
#include <SDL2/SDL_audio.h>
#endif
#include <bits/algorithmfwd.h>
#include "const/play_sync_type.h"
#include "util/sync/clock.h"

struct SDLPlayState {
  friend class SDLVideoPlayer;
  friend class SDLAudioDecodeRegin;
  friend class SDLVideoDecodeRegin;
private:
  uint32_t serial;
  // 以下三个index是播放的流的index，-1表示没有播放
  int32_t lastVidStInd;
  int32_t lastAudStInd;
  int32_t lastSubStInd;
  uint16_t vidStInd;
  uint16_t audStInd;
  uint16_t subStInd;

  uint32_t width;
  uint32_t height;
  uint32_t x;
  uint32_t y; // 这里的x和y是窗口的位置， 以左上角为原点

  bool mute;
  bool paused;
  uint8_t volume; // 音量, 0-128, 因为SDL的音量范围是0-128

  ClockGroup clkGroup;

  PlaySyncType syncType;
  int audioClockSerial;

  bool eof; // 是否到达文件尾
  bool abortReq;

  uint16_t maxFrameDuration; // 最大帧间隔，单位ms

  uint64_t playOffset; // 偏移指定时间开始播放, 单位是AV_TIME_BASE_Q

  uint32_t defaultPicWidth;
  uint32_t defaultPicHeight;

  // private func
  inline void setSDLVolumeUsingPercent(uint8_t vol);
public:
  static constexpr uint8_t sdlMinVolume = 0;
  static constexpr uint8_t sdlMaxVolume = 100;
  explicit SDLPlayState(double playSpeed);
  static int getAborted(void* state);
};

inline int SDLPlayState::getAborted(void* state) {
  return static_cast<SDLPlayState*>(state)->abortReq;
}

inline void SDLPlayState::setSDLVolumeUsingPercent(uint8_t vol) {
  volume = std::clamp(static_cast<uint8_t>(vol / 100.0 * SDL_MIX_MAXVOLUME), static_cast<uint8_t>(0), static_cast<uint8_t>(SDL_MIX_MAXVOLUME));
}


#endif //PLAY_STATE_H
