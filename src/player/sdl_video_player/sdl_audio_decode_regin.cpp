//
// Created by leo on 24-10-15.
//
#include "player/sdl_video_player/sdl_audio_decode_regin.h"

#include <player/sdl_video_player/sdl_video_player.h>

int SDLAudioDecodeRegin::decodeFrame() {
  if (playState.paused) return -1;
  Frame aFrame; // may block
  do {
#ifdef _WIN32
    // TODO:对于这个宏条件的sleep目的，还不太清楚
    while (aFrameq.isEmpty()) {
      if ((Clock::getTimeSameBegin() - callback_time) > 1000000 * hardwareAudBufSize / audTgtParams.getBytePerSec())
        return -1;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
#endif
    aFrame = aFrameq.blockPop();
    // 检测是否已经发出退出信号
    // TODO: 这里也无形中出现了一个要求，要不就在blockPop中随时监测退出信号，要不就不让queue知晓外部
    // TODO: 后者blockPop仍然是阻塞的，他不管你是不是退出，他都会阻塞
    // TODO: 所以就要求再退出信号发出后，需要做一些工作，让所有queue的生产设消费者立刻做出反应，不能死锁
    if (playState.abortReq) {
      return -1; // TODO: 还不知道这里返回值都返回-1行不行，还是需要返回不同的
    }
  } while (aFrame.serial != playState.serial); // 丢弃过时的帧
  int ret = 0;
  uint32_t dataSize = av_samples_get_buffer_size(
    nullptr,
    aFrame.frame->ch_layout.nb_channels,
    aFrame.frame->nb_samples,
    static_cast<AVSampleFormat>(aFrame.frame->format),
    1 // 1 indicates no alignment
  );
  // 为了同步，本次解码的音频帧的理想大小 //TODO: syncAud与ffplay内部实现有出入，怀疑ffplay那里有逻辑错误，之后充分理解后再来看
  uint32_t idealNbSamples = syncAud(aFrame.frame->nb_samples, aFrame.frame->sample_rate);
  if (needPrepareSwrWithFmt(aFrame,idealNbSamples)) {
    // 需要设置重采样器
    if (swrCtx)
      swrCtx.reset();
    SwrContext* swrCtxTmp = nullptr;
    swr_alloc_set_opts2(
      &swrCtxTmp,
      audTgtParams.getChLayoutPtr(),
      audTgtParams.getFmt(),
      audTgtParams.getFreq(),
      &aFrame.frame->ch_layout,
      static_cast<AVSampleFormat>(aFrame.frame->format),
      aFrame.frame->sample_rate,
      0,
      nullptr
    );
    if (!swrCtxTmp || swr_init(swrCtxTmp) < 0) {
      throw ErrorDesc::from(ExceptionType::FFComponentInitFailed, "Cannot create sample rate converter");
    }
    // 现在需要 1.只能指针代管swrCtxTmp, 2. 更新audRecordedParams
    swrCtx.reset(swrCtxTmp);
    updateAudRecord(aFrame);
  }
  // 接下来就是
}


uint32_t SDLAudioDecodeRegin::syncAud(uint32_t nbSamples, uint32_t freq) {
  // 如果当前的同步类型是音频主导，那么我们就不需要做任何处理
  if (playState.clkGroup.getMasterClock() == PlaySyncType::AudioMaster)
    return nbSamples;
  // 到这一步说明当前的同步类型是视频主导或者外部时钟主导
  double diffTime = playState.clkGroup.audClk.getPlayTime() - playState.clkGroup.getMasterClkPlayTime(); // 音频的领先时间，单位是秒，可以为负
  if (std::isnan(diffTime) || std::fabs(diffTime) > tooBigDiffNoSync) {
    // 如果音频领先时间小于最大允许的不同步时间，那么我们就不需要做任何处理
    timeDiff.reset(); // diff太大，就不再累积
    return nbSamples;
  }
  timeDiff.addDiff(diffTime); // 添加时间差
  if (!timeDiff.full()) { // 如果次数不够，那么我们就不需要做任何处理
    return nbSamples;
  }
  // 到这里说明次数统计够了，我们得到了一个平均的时间差
  double diffAvg = timeDiff.getAndReset();
  uint32_t idealNbSamples;
  // 如果时间差太小，那么我们就不需要做任何处理
  if (std::fabs(diffAvg) >= diffThreshToAdjust) {
    idealNbSamples = nbSamples + static_cast<uint32_t>(diffTime * audRecordedParams.getFreq());
    // 下面分情况而不是使用std::clamp，这样节省计算
    if (diffTime < 0) {
      // 说明音频落后，idealNbSamples是小于nbSamples的，这里就要约束不能小得太多
      idealNbSamples = std::max(idealNbSamples, nbSamples * (100 - sampleCorrectionMaxPrecent) / 100);
    }else {
      // 说明音频领先，idealNbSamples是大于nbSamples的，这里就要约束不能大得太多
      idealNbSamples = std::min(idealNbSamples, nbSamples * (100 + sampleCorrectionMaxPrecent) / 100);
    }
  }

  return idealNbSamples;
}
