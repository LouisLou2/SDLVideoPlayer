//
// Created by leo on 24-10-15.
//
#include "player/sdl_video_player/sdl_audio_decode_regin.h"

int SDLAudioDecodeRegin::decodeFrame() {
  if (playState.paused) return -1;
  Frame aFrame; // may block
  do {
#ifdef _WIN32
    // 对于这个宏条件的sleep目的，还不太清楚
    while (aFrameq.isEmpty()) {
      if ((ClockGroup::getTimeSincePowerOn()-callback_time) > 1000000 * hardwareAudBufSize / audTgtParams.getBytePerSec())
        return -1;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
#endif
    aFrame = aFrameq.blockPop();
    // 检测是否已经发出退出信号
    if (playState.abortReq) {
      return -2;
    }
  } while (aFrame.serial != playState.serial); // 丢弃过时的帧

  uint32_t dataSize = av_samples_get_buffer_size(
    nullptr,
    aFrame.frame->ch_layout.nb_channels,
    aFrame.frame->nb_samples,
    static_cast<AVSampleFormat>(aFrame.frame->format),
    1 // 1 indicates no alignment
  );
}


uint32_t SDLAudioDecodeRegin::syncAud(uint32_t nbSamples) {
  uint32_t wantedSampleNum = nbSamples;
  // 如果当前的同步类型是音频主导，那么我们就不需要做任何处理
  if (playState.clkGroup.getMasterClock() == PlaySyncType::AudioMaster)
    return wantedSampleNum;
  // 到这一步说明当前的同步类型是视频主导或者外部时钟主导
  double diffTime = playState.clkGroup.audClk.getPlayTime() - playState.clkGroup.getMasterClkPlayTime(); // 音频的领先时间，单位是秒，可以为负

}

