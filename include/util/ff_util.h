//
// Created by leo on 24-10-6.
//

#ifndef FF_OPT_UTIL_H
#define FF_OPT_UTIL_H
#include <array>
#include <tuple>
#include "player/sdl_video_player/sdl_vid_player_setting.h"
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
  /*
   * fw: frame width, fh: frame height, par: pixel aspect ratio
   * 注意frame width和frame height是帧的宽度像素和高度像素
   */
  static std::tuple<uint32_t,uint32_t> getPictureDisplayRect(uint32_t fw, uint32_t fh, AVRational par, uint32_t wantedWidth, uint32_t wantedHeight);
  // ReatTime != Network-Based
  static bool isRealTime(const AVFormatContext* iformat);
  static inline bool hasDiscontinuousTS(const AVInputFormat* iformat);
  static inline const AVInputFormat* getInputFormat(InputFormatEnum ifmtEnum);
  static inline SeekType decideSeekType(const AVInputFormat* iformat);
  // 返回的vector中的元素是指向AVDictionaryEntry的指针，依附于动态分配的内存，注意使用它的时候不要提前释放内存,所以也不需要手动释放内存，要从原来的AVDictionary中释放
  static std::vector<AVDictionaryEntry*> getAllEntries(const AVDictionary* dict);
  static std::string stringifyAVDictionary(const std::vector<AVDictionaryEntry*>& entries);
  static std::string getAllPairString(const AVDictionary* dict, const char* split = ":");
  static std::string getAllPairString(const std::map<std::string,std::string>& dict, const char* split = ":");
  /*
   * 此函数求解给定sampleRate,和每秒最大调用次数，并且每次call的最小数据量，求解每次调用的数据量, 在实现中还限制了结果必须是2的幂次方
   */
  static uint32_t getSamplesPerSec(uint32_t minSize,uint32_t sampleRate, uint16_t maxCallPerSec);
  static void relayoutChannel(AVChannelLayout* ch_layout, uint8_t num);
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

inline void FFUtil::relayoutChannel(AVChannelLayout* ch_layout, uint8_t num) {
  assert(num > 0);
  av_channel_layout_uninit(ch_layout);
  av_channel_layout_default(ch_layout, num);
}

#endif //FF_OPT_UTIL_H
