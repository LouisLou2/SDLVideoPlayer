//
// Created by leo on 24-10-15.
//
#include "player/sdl_video_player/sdl_audio_decode_regin.h"

void SDLAudioDecodeRegin::decodeFrame() {
  Frame aFrame; // may block
  do {
    aFrame = aFrameq.blockPop();
  } while (aFrame.serial != playState.serial); // 丢弃过时的帧

  uint32_t dataSize = av_samples_get_buffer_size(
    nullptr,
    aFrame.frame->ch_layout.nb_channels,
    aFrame.frame->nb_samples,
    static_cast<AVSampleFormat>(aFrame.frame->format),
    1 // 1 indicates no alignment
  );
}

