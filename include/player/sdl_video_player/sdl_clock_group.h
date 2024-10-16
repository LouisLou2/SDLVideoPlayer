//
// Created by leo on 24-10-15.
//

#ifndef SDL_CLOCK_GROUP_H
#define SDL_CLOCK_GROUP_H
#include <chrono>
#include <cstdint>
#include "util/sync/clock.h"


class ClockGroup {
  friend class SDLPlayState;
  static uint64_t getTimeSincePowerOn();
  Clock audClk;
  Clock vidClk;
  Clock extClk;
public:
  explicit ClockGroup(double speed);
};

inline ClockGroup::ClockGroup(double speed): audClk(speed), vidClk(speed), extClk(speed) {}

inline uint64_t ClockGroup::getTimeSincePowerOn() {
  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

#endif //SDL_CLOCK_GROUP_H
