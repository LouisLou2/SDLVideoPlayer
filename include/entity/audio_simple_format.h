//
// Created by leo on 24-10-20.
//

#ifndef AUDIO_SIMPLE_FORMAT_H
#define AUDIO_SIMPLE_FORMAT_H
#include <cstdint>

#include "audio_params.h"
#include "util/mem/ff_mem.h"
#ifdef __cplusplus
extern "C" {
#include <libavutil/channel_layout.h>
}
#else
#include <libavutil/channel_layout.h>
#endif

class AudioSimpleFormat {
  uint32_t freq;
  AVChannelLayout ch_layout;
  AVSampleFormat fmt;
public:
  inline void setShallowCopy(const AudioParams& ap);
  inline void setDeepCopy(uint32_t freq, const AVChannelLayout* ch_layout, AVSampleFormat fmt);
  [[nodiscard]] inline bool equalTo(uint32_t freq, const AVChannelLayout* ch_layout, AVSampleFormat fmt) const;
  // getters
  [[nodiscard]] uint32_t getFreq() const {return freq;}
  [[nodiscard]] const AVChannelLayout* getChLayoutPtr() const {return &ch_layout;}
  [[nodiscard]] AVSampleFormat getFmt() const {return fmt;}
};

inline void AudioSimpleFormat::setShallowCopy(const AudioParams& ap) {
  freq = ap.getFreq();
  ch_layout = *ap.getChLayoutPtr();
  fmt = ap.getFmt();
}

inline void AudioSimpleFormat::setDeepCopy(uint32_t freq, const AVChannelLayout* ch_layout, AVSampleFormat fmt) {
  this->freq = freq;
  this->fmt = fmt;
  av_channel_layout_copy(&this->ch_layout, ch_layout);
}

inline bool AudioSimpleFormat::equalTo(uint32_t freq, const AVChannelLayout* ch_layout, AVSampleFormat fmt) const {
  return this->freq == freq && this->fmt == fmt && !av_channel_layout_compare(&this->ch_layout, ch_layout);
}

#endif //AUDIO_SIMPLE_FORMAT_H
