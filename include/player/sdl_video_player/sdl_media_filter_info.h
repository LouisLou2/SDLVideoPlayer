//
// Created by leo on 24-10-10.
//

#ifndef SDL_MEDIA_FILTER_INFO_H
#define SDL_MEDIA_FILTER_INFO_H

#include <map>
#include <optional>
#include "entity/error_desc.h"
#include "entity/audio_params.h"
#ifdef __cplusplus
extern "C" {
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
}
#else
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#endif

class SDLMediaFilterInfo {
  friend class SDLVideoPlayer;

  AudioParams audFilterParamsSrc;
  AVFilterContext* audioFilterIn;
  AVFilterContext* audFilterOut;
  AVFilterContext* vidFilterIn;
  AVFilterContext* vidFilterOut;

  AVFilterGraph* audFilterGraph;

  // 构造函数私有，仅供友元类使用
  SDLMediaFilterInfo();
  void setAudFilterParamsSrc(uint32_t freq, const AVChannelLayout* ch_layout, AVSampleFormat fmt);
  // 提供私有函数，只给友元类使用
  std::optional<ErrorDesc> configAudioFilter(
    const std::map<std::string,std::string>& swrOpts,
    const std::string& audioFilterGraphStr,
    uint8_t filterThreadsNum,
    std::optional<const AudioParams*> forcedOutputParams = std::nullopt
  );
  std::optional<ErrorDesc> configAudioFilterGraph(
    const std::string &audioFilterGraphStr,
    AVFilterContext *filterIn,
    AVFilterContext *filterOut
  );
  [[nodiscard]] uint32_t getFilterOutSampleRate() const;
  bool getFilterOutChLayout(AVChannelLayout* targetLayout) const;
};

inline SDLMediaFilterInfo::SDLMediaFilterInfo():
  audFilterGraph(nullptr),
  audioFilterIn(nullptr),
  audFilterOut(nullptr),
  vidFilterIn(nullptr),
  vidFilterOut(nullptr){}

inline void SDLMediaFilterInfo::setAudFilterParamsSrc(
  uint32_t freq,
  const AVChannelLayout* ch_layout,
  AVSampleFormat fmt) {
    audFilterParamsSrc.setFreq(freq);
    audFilterParamsSrc.copySetChLayout(ch_layout);
    audFilterParamsSrc.setFmt(fmt);
}
inline uint32_t SDLMediaFilterInfo::getFilterOutSampleRate() const {
  return av_buffersink_get_sample_rate(audFilterOut);
}
inline bool SDLMediaFilterInfo::getFilterOutChLayout(AVChannelLayout* targetLayout) const {
  return !av_buffersink_get_ch_layout(audFilterOut, targetLayout);
}

#endif //SDL_MEDIA_FILTER_INFO_H
