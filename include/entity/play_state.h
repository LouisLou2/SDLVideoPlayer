//
// Created by leo on 24-10-4.
//

#ifndef PLAY_STATE_H
#define PLAY_STATE_H
#include <cstdint>

#include "const/play_sync_type.h"
#include "util/sync/clock.h"

struct PlayState {
  friend class SDLVideoPlayer;
private:
  // 以下三个index是播放的流的index，-1表示没有播放
  int16_t lastVidStInd;
  int16_t lastAudStInd;
  int16_t lastSubStInd;
  uint16_t vidStInd;
  uint16_t audStInd;
  uint16_t subStInd;

  uint32_t width;
  uint32_t height;
  uint32_t x;
  uint32_t y; // 这里的x和y是窗口的位置， 以左上角为原点

  Clock audClk;
  Clock vidClk;
  Clock extClk;

  bool mute;
  bool paused;
  uint16_t volume;

  PlaySyncType syncType;
  int audioClockSerial;
public:
  PlayState();
};



#endif //PLAY_STATE_H
