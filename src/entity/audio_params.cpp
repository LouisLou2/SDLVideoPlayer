//
// Created by leo on 24-10-10.
//

#include "entity/audio_params.h"
#include <cassert>
#include <limits>

AudioParams& AudioParams::operator=(const AudioParamsDTO& dto) {
  assert(dto.ch_layout);
  freq = dto.freq;
  fmt = dto.fmt;
  av_channel_layout_copy(&ch_layout, dto.ch_layout);
  frameSize = dto.frameSize;
  bytePerSec = dto.bytePerSec;
  return *this;
}

void AudioParams::shallowCopy(const AudioParams& ap) {
  this->freq = ap.freq;
  this->ch_layout = ap.ch_layout;
  this->fmt = ap.fmt;
  this->frameSize = ap.frameSize;
  this->bytePerSec = ap.bytePerSec;
}

void AudioParams::setParams(
  uint32_t freq,
  const AVChannelLayout* ch_layout,
  AVSampleFormat fmt) {

  assert(ch_layout && freq > 0);

  this->freq = freq;
  this->fmt = fmt;
  av_channel_layout_copy(&this->ch_layout, ch_layout);
  frameSize = av_samples_get_buffer_size(nullptr, ch_layout->nb_channels, 1, fmt, 1); // 1 indicates no alignment
  assert(frameSize <= std::numeric_limits<uint32_t>::max() / freq);
  bytePerSec = frameSize * freq;
}
