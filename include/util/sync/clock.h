//
// Created by leo on 24-10-4.
//

#ifndef CLOCK_H
#define CLOCK_H

#include <limits>
#include <cstdint>
#include <chrono>

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
  const uint32_t& latestSerialRef; // 最新的序列号参照
  // uint32_t* queueSerial;

  // 这个构造函数是默认构造，它建立的对象目前是不能使用的
  inline explicit Clock(double speed, const uint32_t& latestSerialRef);
public:
  // 不允许拷贝构造和赋值
  Clock(const Clock&) = delete;
  Clock& operator=(const Clock&) = delete;
  /*
   * @brief 获取当前时间，这个时间是选定一个起点，然后计算到现在的时间
   * @return 当前相对时间，单位是微秒
   * @note 这个时间是相对于一个固定的起点，这个起点到底是什么，其实是不重要的，只要这个起点是固定的就行，
   * 这里默认实现选择系统开机时间
   */
  static uint64_t getTimeSameBegin();
  double getPlayTime() const;
};

// aware: 任何有nan参与的运算都会返回nan
inline Clock::Clock(double speed, const uint32_t& latestSerialRef) :
  pts(std::numeric_limits<double>::quiet_NaN()),
  ptsDrift(pts), // pts和ptsDrift的初始值都是NaN
  lastUpdated(pts), //转换为秒为单位
  speed(speed),
  paused(false),
  serial(-1),
  latestSerialRef(latestSerialRef) {}

inline uint64_t Clock::getTimeSameBegin() {
  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

#endif //CLOCK_H
