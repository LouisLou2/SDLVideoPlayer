//
// Created by leo on 24-10-4.
//

#include "entity/play_state.h"

PlayState::PlayState():
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
  audClk(),
  vidClk(),
  extClk(),
  mute(false),
  paused(false),
  volume(100),
  syncType(PlaySyncType::AudioMaster),
  audioClockSerial(0){
}
