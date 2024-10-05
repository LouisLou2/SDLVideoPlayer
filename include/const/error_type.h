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

#define EXCEPTNUM_DEF 11

// 必须是uint16_t类型, type又有多种分类，warn error critical
enum class ExceptionType : uint16_t { // 更改这里记得更改ExceptionTypeUtil的typeNum！以及下边一系列的方法
  /*----critical----*/
  Unknown = 0,
  SystemError = 1,
  NoneComplaint = 2,
  /*----error----*/
  UnsupportedFormat = 3,
  InvalidArgument = 4,

  ResourceNotFound = 5,
  FileUnreadable = 6,
  BrokenStream = 7,
  SDLInitFailed = 8,
  SDLComponentInitFailed = 9,
  /*----warn----*/
  SDLHardwareAccelerationFailed = 10,
};

struct ExceptionTypeUtil {
  static constexpr uint16_t typeNum = EXCEPTNUM_DEF;
  static constexpr uint16_t criticalMax = 2;
  static constexpr uint16_t errorMax = 9;
  static constexpr std::array<std::string_view, typeNum> typeStr = {
    /*----critical----*/
    "Unknown", // 0
    "System Error", // 1
    "None Complaint", // 2
    /*----error----*/
    "Unsupported Format", //3
    "Invalid Argument", //4
    "Resource Not Found", //5
    "File Unreadable", //6
    "Broken Stream", //7
    "SDL Init Failed", //8
    "SDL Component Init Failed", //9
    /*----warn----*/
    "SDL Hardware Acceleration Failed", //10
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
