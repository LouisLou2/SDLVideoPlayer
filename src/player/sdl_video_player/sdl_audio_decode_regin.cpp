//
// Created by leo on 24-10-15.
//
#include "player/sdl_video_player/sdl_audio_decode_regin.h"

#include <player/sdl_video_player/sdl_video_player.h>

#include "util/mem/mem_alloc.h"

// 返回值可以试试std::expected应用在这个函数中每个返回-1的地方
int SDLAudioDecodeRegin::decodeFrame() {

  //只能是这个格式，下面都是基于这个格式才成立的
  assert(audTgtParams.getFmt() == SDLVidPlayerSettings::theOnlyAllowedSRFmt_FF);

  if (playState.paused) return -1;
  do {
#ifdef _WIN32
    // TODO:对于这个宏条件的sleep目的，还不太清楚
    while (latestFq.isEmpty()) {
      if ((Clock::getTimeSameBegin() - callback_time) > 1000000 * hardwareAudBufSize / audTgtParams.getBytePerSec())
        return -1;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
#endif
    latestF = aFrameq.blockPop(); //一旦pop出来，Frame的管理责任唯一地就交给了这里
    // 检测是否已经发出退出信号
    // TODO: 这里也无形中出现了一个要求，要不就在blockPop中随时监测退出信号，要不就不让queue知晓外部
    // TODO: 后者blockPop仍然是阻塞的，他不管你是不是退出，他都会阻塞
    // TODO: 所以就要求再退出信号发出后，需要做一些工作，让所有queue的生产设消费者立刻做出反应，不能死锁
    if (playState.abortReq) {
      return -1; // TODO: 还不知道这里返回值都返回-1行不行，还是需要返回不同的
    }
  } while (latestF.serial != playState.serial); // 丢弃过时的帧

  int ret = 0;
  int outSamplePerCh = 0;
  // 为了同步，本次解码的音频帧的理想大小 //TODO: syncAud与ffplay内部实现有出入，怀疑ffplay那里有逻辑错误，之后充分理解后再来看
  uint32_t idealNbSamples = syncAud(latestF.frame->nb_samples, latestF.frame->sample_rate);
  bool needPrepareSwr = needPrepareSwrWithFmt(latestF, idealNbSamples);

  if (needPrepareSwr) {
    // 需要设置重采样器
    if (swrCtx)
      swrCtx.reset();
    SwrContext* swrCtxTmp = nullptr;
    swr_alloc_set_opts2(
      &swrCtxTmp,
      audTgtParams.getChLayoutPtr(),
      audTgtParams.getFmt(),
      audTgtParams.getFreq(),
      &latestF.frame->ch_layout,
      static_cast<AVSampleFormat>(latestF.frame->format),
      latestF.frame->sample_rate,
      0,
      nullptr
    );
    if (!swrCtxTmp || swr_init(swrCtxTmp) < 0) {
      throw ErrorDesc::from(ExceptionType::FFComponentInitFailed, "Cannot create sample rate converter");
    }
    // 现在需要 1.只能指针代管swrCtxTmp, 2. 更新audRecordedParams
    swrCtx.reset(swrCtxTmp);
    updateAudRecord(latestF);
  }
  // 接下来就是配置平滑补偿以及进行采样
  // 如果needPrepareSwr为false，此时swrCtx当然也可能不为nullptr，因为可能是之前用到的
  // 如果这里swrCtx不为nullptr，就配置重采样，
  // 会导致如果needPrepareSwr为false，且idealNbSamples与latestF.frame->nb_samples相同，也会进行重采样
  // 这就是没必要的
  if (!needPrepareSwr && idealNbSamples==latestF.frame->nb_samples) {
    uint32_t dataSize = av_samples_get_buffer_size(
      nullptr,
      latestF.frame->ch_layout.nb_channels,
      latestF.frame->nb_samples,
      static_cast<AVSampleFormat>(latestF.frame->format),
      1 // 1 indicates no alignment
    );
    // 我们写死了音频帧是packed形式，所以直接data[0]指向的就是全部数据了，
    // 这里直接将data指向latestF.frame->data[0]，// TODO: 但是latestF该如何释放，还没有想好
    data = std::span(latestF.frame->data[0], dataSize);
  } else {
    // 说明要不就是格式不一致，要不就是idealNbSamples与latestF.frame->nb_samples不一致，这里就要重采样
    /*
     * 我真是服了，为了和ffmpeg api兼容，swr_convert如果要表示不修改in，那么in就要设置成const uint8_t* const*，这样防止使用const_cast直接static_cast搞定了。
     */
    const uint8_t** indata = const_cast<const uint8_t**>(latestF.frame->extended_data);
    uint8_t** out = &_processedData; // out既可以改变指向的内容，也可以改变指向的指针
    uint32_t preciseWantedOutSize = idealNbSamples * audTgtParams.getFreq() / latestF.frame->sample_rate;
    uint32_t maxSamplePerChBufCanHold = preciseWantedOutSize + sampleNumMargin;
    int bufSize = av_samples_get_buffer_size(
      nullptr,
      audTgtParams.getChLayoutPtr()->nb_channels,
      maxSamplePerChBufCanHold,
      audTgtParams.getFmt(),
      0
    );
    if (bufSize < 0) {
      return -1;
    }
    if (idealNbSamples != latestF.frame->nb_samples) {
      // 说明idealNbSamples与latestF.frame->nb_samples不一致，这里就要平滑补偿
      /*
       * 平滑补偿，sample_delta是要补偿的样本数，compensation_distance是补偿之后要得到的样本数，注意它们的样本数是基于一个通道而言的
       */
      ret = swr_set_compensation(
        swrCtx.get(),
        (idealNbSamples - latestF.frame->nb_samples) * audTgtParams.getFreq() / latestF.frame->sample_rate,
        preciseWantedOutSize
      );
      if (ret < 0) {
        return -1;
      }
    }
    MemAlloc::tryReuse(_processedData, _processedDataSize, bufSize);
    /*
     * 为什么swr知道音频帧是planar还是packed？ 因为alloc_set_opts2的时候，我们已经指定了, 通过fmt
     * swr也知道通道的layout
     */
    outSamplePerCh = swr_convert(
      swrCtx.get(),
      out,
      maxSamplePerChBufCanHold,
      indata,
      latestF.frame->nb_samples
    );
    if (outSamplePerCh < 0) {
      return -1;
    }
    if (outSamplePerCh == maxSamplePerChBufCanHold) {
      // 很可能是缓冲区不够大，需要警告
      // 之前的从采样可能并没有正确完成，这里重置它
      if (swr_init(swrCtx.get()) < 0)
        // 重置也不成功，就释放
        swrCtx.reset();
    }
    this->data = std::span(
      this->_processedData,
      outSamplePerCh * audTgtParams.getChLayoutPtr()->nb_channels * bytePerSampleForTgt
    );
  }

  // 设置时钟
  //TODO： ffplay中 is->audio_clock_serial是这里被设置了，但是我还不知道这个变量的作用，所以暂时没有设置，一定要弄清楚
  if (std::isnan(latestF.pts))
    whenNewestFrameFinished = std::numeric_limits<double>::quiet_NaN();
  else
    whenNewestFrameFinished = latestF.pts + outSamplePerCh / static_cast<double>(audTgtParams.getFreq()); // TODO: 这里的实现和ffplay有出入，但是我觉得ffplay的实现有问题

  return data.size();
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
