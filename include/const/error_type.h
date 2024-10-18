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

// 必须是uint16_t类型, type又有多种分类，warn error critical
enum class ExceptionType : uint16_t { // 更改这里记得更改ExceptionTypeUtil的typeNum！以及下边一系列的方法
  /*----critical----*/
  Unknown = 0,
  SystemError,
  NoneComplaint,
  MemoryAllocFailed,
  Unimplemented,
  UnsupportedHardware,
  /*----error----*/
  UnsupportedFormat,
  InvalidArgument,
  ResourceNotFound,
  FileUnreadable,
  BrokenStream,
  SDLInitFailed,
  SDLComponentInitFailed,
  FFmpegSetFailed,
  FFComponentInitFailed,
  UnusedFFOption,
  ContainNoStream,
  NoAudioAndVideo,
  /*----warn----*/
  SDLHardwareAccelerationFailed,
  AutoAdjust,
  PlaySeekFailed,
  WantedStreamNotFound,
  UseOtherDecoder,
  UnsupportedLowResLevel,
  BigConfiguration
};

struct ExceptionTypeUtil {
  static constexpr uint16_t typeNum = static_cast<uint16_t>(ExceptionType::BigConfiguration) + 1;
  static constexpr uint16_t criticalMax = static_cast<uint16_t>(ExceptionType::UnsupportedHardware);
  static constexpr uint16_t errorMax = static_cast<uint16_t>(ExceptionType::NoAudioAndVideo);
  static constexpr std::array<std::string_view, typeNum> typeStr = {
    /*----critical----*/
    "Unknown", // 0
    "System Error",
    "None Complaint",
    "Memory Alloc Failed",
    "Unimplemented",
    "Unsupported Hardware",
    /*----error----*/
    "Unsupported Format",
    "Invalid Argument",
    "Resource Not Found",
    "File Unreadable",
    "Broken Stream",
    "SDL Init Failed",
    "SDL Component Init Failed",
    "FFmpeg Set Failed",
    "FF Component Init Failed",
    "Unused FF Option",
    "Contain No Stream",
    "No Audio And Video",
    /*----warn----*/
    "SDL Hardware Acceleration Failed",
    "Auto Adjust",
    "Play Seek Failed",
    "Wanted Stream Not Found",
    "Use Other Decoder",
    "Unsupported Low Res Level",
    "Big Configuration"
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
