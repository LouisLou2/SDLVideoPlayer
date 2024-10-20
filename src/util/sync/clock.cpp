//
// Created by leo on 24-10-20.
//
#include "util/sync/clock.h"

double Clock::getPlayTime() const {
  if (serial!=latestSerialRef) {
    // TODO: 这里就是用来标记过时的，但是具体机制还不太清楚，之后再琢磨
    return std::numeric_limits<double>::quiet_NaN();
  }
  if (paused) {
    //TODO: 这里直接返回pts,但是这是上一次更新的pts,这样做可能是不准确的，所以其他地方会做额外的操作，例如stream_toggle_pause
    return pts;
  }
  double now = getTimeSameBegin() / 1000000.0; // microseconds to seconds
  return now + pts + (now - lastUpdated) * (speed - 1);
}
