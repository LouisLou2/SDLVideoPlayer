//
// Created by leo on 24-9-30.
//

#ifndef SUBTITLE_TYPE_H
#define SUBTITLE_TYPE_H
#include "entity/error_desc.h"

enum class SubtitleTypeEnum : uint8_t {
  None = 0,
  Bitmap = 1,
  Text = 2,
  Ass = 3,
};

class SubtitleTypeUtil {
  inline static SubtitleTypeEnum getTypeEnum(int type);
};

inline SubtitleTypeEnum SubtitleTypeUtil::getTypeEnum(int type) {
  if (0<=type && type <= 3) return static_cast<SubtitleTypeEnum>(type);
  throw ErrorDesc::from(ExceptionType::UnsupportedFormat, "Unsupported SubtitleType");
}
#endif //SUBTITLE_TYPE_H
