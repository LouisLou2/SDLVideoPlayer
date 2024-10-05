//
// Created by leo on 24-10-5.
//

#ifndef INPUT_FORMAT_ENUM_H
#define INPUT_FORMAT_ENUM_H
#include <array>
#include <cassert>
#include <cstdint>

#define INPUT_FORMAT_ENUM_COUNT 9

enum class InputFormatEnum :uint8_t {
  MP4 = 0,
  FLV = 1,
  MKV = 2,
  AVI = 3,
  MOV = 4,
  RMVB = 5,
  WMV = 6,
  WEBM = 7,
  M3U8 = 8,
};

struct InputFormatEnumUtil {
  static constexpr uint8_t typeNum = INPUT_FORMAT_ENUM_COUNT;
  static constexpr std::array<const char*, typeNum> typeStr = {
    "mp4",
    "flv",
    "mkv",
    "avi",
    "mov",
    "rmvb",
    "wmv",
    "webm",
    "m3u8",
  };
  inline static const char* toStr(InputFormatEnum type);
};

inline const char* InputFormatEnumUtil::toStr(InputFormatEnum type) {
  assert(static_cast<uint8_t>(type) < typeNum);
  return typeStr.at(static_cast<uint8_t>(type));
}

#endif //INPUT_FORMAT_ENUM_H
