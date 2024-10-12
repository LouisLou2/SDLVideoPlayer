//
// Created by leo on 24-10-10.
//

#ifndef AUDIO_PARAMS_H
#define AUDIO_PARAMS_H
#include <cstdint>
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
  // inline AudioParams(
  //   uint32_t freq,
  //   AVChannelLayout channelLayout,
  //   AVSampleFormat fmt,
  //   uint32_t frameSize,
  //   uint32_t bytePerSec);
  void setFreq(uint32_t freq) {this->freq = freq;}
  void  copySetChLayout(const AVChannelLayout* ch_layout) {
    av_channel_layout_copy(&this->ch_layout, ch_layout);
  }
  void setFmt(AVSampleFormat fmt) {this->fmt = fmt;}
};

// inline AudioParams::AudioParams(
//   uint32_t freq,
//   AVChannelLayout channelLayout,
//   AVSampleFormat fmt,
//   uint32_t frameSize,
//   uint32_t bytePerSec):
// freq(freq),
// channelLayout(channelLayout),
// fmt(fmt),
// frameSize(frameSize),
// bytePerSec(bytePerSec) {}

#endif //AUDIO_PARAMS_H
