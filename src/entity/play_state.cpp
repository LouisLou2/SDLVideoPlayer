//
// Created by leo on 24-10-4.
//

#include "entity/play_state.h"
#ifdef __cplusplus
extern "C" {
#include <SDL_audio.h>
}
#else
#include <SDL_audio.h>
#endif

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
  mute(false),
  paused(false),
  volume(SDL_MIX_MAXVOLUME / 2),
  syncType(PlaySyncType::AudioMaster),
  audioClockSerial(-1),
  eof(false), // 默认没有到达文件尾
  abortReq(false), // 默认没有退出请求
  maxFrameDuration(0),// 0是非法的，这个也必须后来改动
  playOffset(0){
}
