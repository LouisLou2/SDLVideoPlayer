//
// Created by leo on 24-10-15.
//
#include "player/sdl_video_player/sdl_clock_group.h"
double ClockGroup::getMasterClkPlayTime() const {
  switch (syncType) {
    case PlaySyncType::AudioMaster:
      return audClk.getPlayTime();
    case PlaySyncType::VideoMaster:
      return vidClk.getPlayTime();
    case PlaySyncType::ExternalMaster:
      return extClk.getPlayTime();
    default:
      // 由于syncType是一个枚举类型，所有的枚举值都是有意义的，所以这里的default分支是不会被执行的
      return std::numeric_limits<double>::quiet_NaN();
  }
}
