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

#define EXCEPTNUM_DEF 5

// 必须是uint16_t类型, type又有多种分类，warn error critical
enum class ExceptionType : uint16_t {
  /*----critical----*/
  Unknown = 0,
  SystemError = 1,
  /*----error----*/
  UnsupportedFormat = 2,
  InvalidArgument = 3,
  ResourceNotFound = 4,
  /*----warn----*/
};

struct ExceptionTypeUtil {
  static constexpr uint16_t typeNum = EXCEPTNUM_DEF;
  static constexpr std::array<std::string_view, typeNum> typeStr = {
    /*----critical----*/
    "Unknown", // 0
    "SystemError", // 1
    /*----error----*/
    "UnsupportedFormat", //2
    "InvalidArgument", //3
    "ResourceNotFound", //4
    /*----warn----*/
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
  if (code < 2) return LogLevel::Critical;
  if (code < 5) return LogLevel::Error;
  return LogLevel::Warn;
}

inline uint16_t ExceptionTypeUtil::typeCode(ExceptionType type) {
  assert(static_cast<uint16_t>(type) < typeNum);
  return static_cast<uint16_t>(type);
}

#endif //ERROR_TYPE_H
