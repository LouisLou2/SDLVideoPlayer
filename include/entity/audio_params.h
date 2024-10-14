//
// Created by leo on 24-10-10.
//

#ifndef AUDIO_PARAMS_H
#define AUDIO_PARAMS_H
#include <cassert>
#include <cstdint>
#include <limits>
#ifdef __cplusplus
extern "C" {
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
}
#else
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#endif


class AudioParams {
  friend class SDLMediaFilterInfo;
  friend class SDLVideoPlayer;
  uint32_t freq;
  AVChannelLayout ch_layout;
  AVSampleFormat fmt;
  uint32_t frameSize;
  uint32_t bytePerSec;
public:
  void setFreq(uint32_t freq) {this->freq = freq;}
  void  copySetChLayout(const AVChannelLayout* ch_layout) {
    av_channel_layout_copy(&this->ch_layout, ch_layout);
  }
  void setFmt(AVSampleFormat fmt) {this->fmt = fmt;}
  void setSizes(uint32_t frameSize, uint32_t framePerSec) {
    // bytePerSec应该不会超过2^32
    assert(frameSize > 0 && framePerSec > 0);
    this->frameSize = frameSize;
    uint64_t bytePerSec = frameSize * framePerSec;
    assert(bytePerSec <= std::numeric_limits<uint32_t>::max());
    this->bytePerSec = static_cast<uint32_t>(bytePerSec);
  }
};

#endif //AUDIO_PARAMS_H
