//
// Created by leo on 24-10-4.
//

#ifndef CLOCK_H
#define CLOCK_H
#include <cstdint>

struct Clock {
  double pts;
  double ptsDrift;
  double lastUpdated;
  double speed;
  bool paused;
  uint32_t serial;
  // uint32_t* queueSerial;
};

#endif //CLOCK_H
