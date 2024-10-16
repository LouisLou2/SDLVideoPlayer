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

//
struct Clock {
  double pts;
  double ptsDrift;
  double lastUpdated;
  double speed;
  bool paused;
  uint32_t serial;
  // uint32_t* queueSerial;

  // 这个构造函数是默认构造，它建立的对象目前是不能使用的
  inline explicit Clock(double speed);
};

// aware: 任何有nan参与的运算都会返回nan
inline Clock::Clock(double speed):
  pts(std::numeric_limits<double>::quiet_NaN()),
  ptsDrift(pts), // pts和ptsDrift的初始值都是NaN
  lastUpdated(pts), //转换为秒为单位
  speed(speed),
  paused(false),
  serial(-1) {}

#endif //CLOCK_H
