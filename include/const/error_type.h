//
// Created by leo on 24-9-30.
//

#ifndef ERROR_TYPE_H
#define ERROR_TYPE_H
#include <array>
#include <cassert>
#include <cstdint>
#include <string_view>

#include "log_level.h"

#define EXCEPTNUM_DEF 14

// 必须是uint16_t类型, type又有多种分类，warn error critical
enum class ExceptionType : uint16_t { // 更改这里记得更改ExceptionTypeUtil的typeNum！以及下边一系列的方法
  /*----critical----*/
  Unknown = 0,
  SystemError = 1,
  NoneComplaint = 2,
  MemoryAllocFailed = 3,
  /*----error----*/
  UnsupportedFormat = 4,
  InvalidArgument = 5,

  ResourceNotFound = 6,
  FileUnreadable = 7,
  BrokenStream = 8,
  SDLInitFailed = 9,
  SDLComponentInitFailed = 10,
  /*----warn----*/
  SDLHardwareAccelerationFailed = 11,
  AutoAdjust = 12,
  PlaySeekFailed = 13
};

struct ExceptionTypeUtil {
  static constexpr uint16_t typeNum = EXCEPTNUM_DEF;
  static constexpr uint16_t criticalMax = 3;
  static constexpr uint16_t errorMax = 10;
  static constexpr std::array<std::string_view, typeNum> typeStr = {
    /*----critical----*/
    "Unknown", // 0
    "System Error", // 1
    "None Complaint", // 2
    "Memory Alloc Failed", // 3
    /*----error----*/
    "Unsupported Format", //4
    "Invalid Argument", //5
    "Resource Not Found", //6
    "File Unreadable", //7
    "Broken Stream", //8
    "SDL Init Failed", //9
    "SDL Component Init Failed", //10
    /*----warn----*/
    "SDL Hardware Acceleration Failed", //11
    "Auto Adjust" //12
    "Play Seek Failed" //13
};
  inline static std::string_view toStr(ExceptionType type);
  inline static LogLevel getLogLevel(ExceptionType type);
  inline static uint16_t typeCode(ExceptionType type);
};


inline std::string_view ExceptionTypeUtil::toStr(ExceptionType type) {
  assert(static_cast<uint16_t>(type) < typeNum);
  return typeStr.at(static_cast<uint8_t>(type));
}

inline LogLevel ExceptionTypeUtil::getLogLevel(ExceptionType type) {
  auto code = static_cast<uint16_t>(type);
  if (code <= criticalMax) return LogLevel::Critical;
  if (code <= errorMax) return LogLevel::Error;
  return LogLevel::Warn;
}

inline uint16_t ExceptionTypeUtil::typeCode(ExceptionType type) {
  assert(static_cast<uint16_t>(type) < typeNum);
  return static_cast<uint16_t>(type);
}

#endif //ERROR_TYPE_H
