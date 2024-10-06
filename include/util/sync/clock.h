//
// Created by leo on 24-10-4.
//

#ifndef CLOCK_H
#define CLOCK_H

#include <limits>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#include<libavutil/time.h>
}
#else
#include<libavutil/time.h>
#endif

struct Clock {
  double pts;
  double ptsDrift;
  double lastUpdated;
  double speed;
  bool paused;
  uint32_t serial;
  // uint32_t* queueSerial;
  inline Clock();
};

inline Clock::Clock():
pts(std::numeric_limits<double>::quiet_NaN()),
ptsDrift(pts), // pts和ptsDrift的初始值都是NaN
lastUpdated(av_gettime_relative() / 1000000.0), //转换为秒为单位
speed(1.0),
paused(false),
serial(-1) {}

#endif //CLOCK_H
