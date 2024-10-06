//
// Created by leo on 24-10-6.
//

#ifndef FF_OPT_UTIL_H
#define FF_OPT_UTIL_H
#include <vector>
#include <entity/player_setting.h>
#ifdef __cplusplus
extern "C" {
#include <libavutil/dict.h>
#include <libavformat/avformat.h>
}
#else
#include <libavutil/dict.h>
#include <libavformat/avformat.h>
#endif

class FFUtil {
  static constexpr std::array<const char*,3> realTimeFormats = {
    "rtp",
    "rtsp",
    "sdp"
  };
public:
  // ReatTime != Network-Based
  static bool isRealTime(const AVFormatContext* iformat);
  static inline bool hasDiscontinuousTS(const AVInputFormat* iformat);
  static inline const AVInputFormat* getInputFormat(InputFormatEnum ifmtEnum);
  static inline SeekType decideSeekType(const AVInputFormat* iformat);
  static AVDictionary* parseFormatOpt(const PlayerSettings& settings);
  static std::vector<AVDictionary*> parseCodecOpt(const AVFormatContext* fmtCtx, const PlayerSettings& settings);
};

inline bool FFUtil::hasDiscontinuousTS(const AVInputFormat* iformat) {
  return iformat->flags & AVFMT_TS_DISCONT;
}

inline const AVInputFormat* FFUtil::getInputFormat(InputFormatEnum ifmtEnum) {
  return av_find_input_format(InputFormatEnumUtil::toStr(ifmtEnum));
}

inline SeekType FFUtil::decideSeekType(const AVInputFormat* iformat) {
  return !(iformat->flags & AVFMT_NO_BYTE_SEEK) // 该格式支持字节定位
         &&                                             // 且
          (iformat->flags & AVFMT_TS_DISCONT)   // 该格式可能存在时间戳不连续
         &&                                            // 且
          strcmp("ogg", iformat->name) != 0    // 该格式不是ogg
         ? SeekType::ByByte : SeekType::ByTime;
}
#endif //FF_OPT_UTIL_H
