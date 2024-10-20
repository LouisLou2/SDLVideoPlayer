//
// Created by leo on 24-10-20.
//
#include "entity/audio_param_dto.h"

#include <limits>
#include <cassert>

AudioParamsDTO::AudioParamsDTO(
  uint32_t freq,
  const AVChannelLayout* ch_layout,
  AVSampleFormat fmt):
  freq(freq),
  ch_layout(ch_layout),
  fmt(fmt){

  assert(ch_layout && freq > 0);
  frameSize = av_samples_get_buffer_size(nullptr, ch_layout->nb_channels, 1, fmt, 1); // 1 indicates no alignment
  assert(frameSize <= std::numeric_limits<uint32_t>::max() / freq);
  bytePerSec = frameSize * freq;
}

// void AudioParamsDTO::setParams(
//   uint32_t freq,
//   const AVChannelLayout* ch_layout,
//   AVSampleFormat fmt,
//   uint32_t frameSize,
//   uint32_t framePerSec) {
//   assert(ch_layout);
//   assert(frameSize > 0 && framePerSec > 0);
//   assert(frameSize <= std::numeric_limits<uint32_t>::max() / framePerSec);
//   this->freq = freq;
//   this->ch_layout = ch_layout;
//   this->fmt = fmt;
//   this->frameSize = frameSize;
//   this->bytePerSec = frameSize * framePerSec;
// }
