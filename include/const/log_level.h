//
// Created by leo on 24-10-2.
//

#ifndef LOG_LEVEL_H
#define LOG_LEVEL_H

// 现在各个级别是没有什么顺序要求的
enum class LogLevel: uint8_t {
  Debug,
  Trace,
  Info,
  Warn,
  Error,
  Critical,
};

#endif //LOG_LEVEL_H
