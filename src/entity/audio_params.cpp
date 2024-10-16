//
// Created by leo on 24-10-10.
//

#include "entity/audio_params.h"

void AudioParams::setSizes(uint32_t frameSize, uint32_t framePerSec) {
  // bytePerSec应该不会超过2^32
  assert(frameSize > 0 && framePerSec > 0);
  this->frameSize = frameSize;
  uint64_t bytePerSec = frameSize * framePerSec;
  assert(bytePerSec <= std::numeric_limits<uint32_t>::max());
  this->bytePerSec = static_cast<uint32_t>(bytePerSec);
}
