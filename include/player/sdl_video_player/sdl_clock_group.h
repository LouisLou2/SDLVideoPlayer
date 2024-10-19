//
// Created by leo on 24-10-15.
//

#ifndef SDL_CLOCK_GROUP_H
#define SDL_CLOCK_GROUP_H
#include <cstdint>
#include "const/play_sync_type.h"

#include "util/sync/clock.h"


class ClockGroup {
  friend class SDLPlayState;
  friend class SDLAudioDecodeRegin;
  PlaySyncType syncType; // 即masterClock
  Clock audClk;
  Clock vidClk;
  Clock extClk;
public:
  explicit ClockGroup(double speed, const uint32_t& audqSerialRef, const uint32_t& vidqSerialRef, const uint32_t& extqSerialRef);
  inline void setMasterClock(PlaySyncType syncType);
  [[nodiscard]] inline PlaySyncType getMasterClock() const;
  [[nodiscard]] double getMasterClkPlayTime() const;
};

inline ClockGroup::ClockGroup(double speed, const uint32_t& audqSerialRef, const uint32_t& vidqSerialRef, const uint32_t& extqSerialRef): audClk(speed, audqSerialRef), vidClk(speed,audqSerialRef), extClk(speed, audqSerialRef) {}

inline void ClockGroup::setMasterClock(PlaySyncType syncType) {
  this->syncType = syncType;
}
inline PlaySyncType ClockGroup::getMasterClock() const {
  return syncType;
}
#endif //SDL_CLOCK_GROUP_H
