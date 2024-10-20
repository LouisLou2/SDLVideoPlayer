//
// Created by leo on 24-10-15.
//

#ifndef SDL_AUDIO_DECODE_REGIN_H
#define SDL_AUDIO_DECODE_REGIN_H

#include "util/mem/ff_mem.h"
#include "diff_time.h"
#include "sdl_play_state.h"
#include "struct/frame_queue.h"
#include "entity/audio_params.h"
#include "entity/audio_param_dto.h"
#include "entity/audio_simple_format.h"

// using setTgtByDisplayDecisionFunc = std::function<void(
//   uint32_t freq,
//   const AVChannelLayout* ch_layout,
//   AVSampleFormat fmt,
//   uint32_t frameSize,
//   uint32_t framePerSec,
//   uint32_t hardwareAudBufSize
// )>;

class SDLAudioDecodeRegin {
  static constexpr uint8_t sampleCorrectionMaxPrecent = 10;
  /*----------用于同步的---------------*/
  // 太大的时间差，不进行同步,怀疑是因为文件pts有问题
  const double tooBigDiffNoSync;
  TimeDiff timeDiff;
  // 用于调整音频同步的阈值,这个阈值是根据音频的帧大小和硬件音频缓冲区大小计算出来的，也许有更加科学的方法
  double diffThreshToAdjust;
  uint64_t callback_time;
  //TODO： 当最新音频帧都被播放完毕的时间， don't know how to use for now
  double whenNewestFrameFinished;
  /*--------音频的信息-----------------*/
  // 音频的源参数和目标参数
  // bool audRealParamsSet = false; // 这里不适用这个，是考虑到一个流中可能前后格式不一样，即使这个可能性很小
  AudioSimpleFormat audRecordedParams; // 已经创建好应对方案的音频格式，一开始就是和audTgtParams一样的
  // 目标参数, 足以他只能被设置一次
  AudioParams audTgtParams;
  /*--------队列----------------------*/
  FrameQueue& aFrameq;
  /*--------硬件相关与播放状态-----------*/
  uint64_t hardwareAudBufSize;
  /*
   * @brief 下面是用于音视频播放控制的变量，他们集结在SDLVideoPlayer的PlayState中，这里我们引用他(const形式)
   * TODO: 实际上不是其中的所有值都会使用到，这样直接引用会导致不必要的耦合，后续可以考虑将其中的一部分值提取出来
   */
  const SDLPlayState& playState;
  /*-------------重采样-----------------*/
  std::unique_ptr<SwrContext, SwrContextDeleter> swrCtx;
public:
  // cons
  SDLAudioDecodeRegin(
    FrameQueue& aFrameq,
    const SDLPlayState& playState,
    double tooBigDiffNoSync,
    uint16_t numDiffSamplesToSync,
    double weightOfOldestDiff
  );

  /*
   * @brief 此函数拿到Display最终决定的音频参数以及硬件音频缓冲区的大小
   * @param audioParamsDTO Display最终决定的音频参数
   * @param hardwareAudBufSize 硬件音频缓冲区的大小
   * @note  AudioParamsDTO中的ch_layout指针指向的资源生命周期如何我们不知，我们要复制他的内容
   */
  inline void setFromDisplayDecision(const AudioParamsDTO& audioParamsDTO, uint32_t hardwareAudBufSize);

  int decodeFrame();

  /*
   *  @brief 用于音频同步，因为音频可能会落后或者领先于主时钟，
   *  例如落后了，那么这里的样本就应该减小一点，使得实际播放的样本数少，但是我们记录的时间就是原本的时间，这样就可以尽快追上
   *  @param nbSamples 本次解码的音频帧的样本数
   *  @param freq 本次解码的音频帧的采样率
   *  @return 为了达到同步目的，理想的音频帧数应该是多少(注意是在本次解码的音频帧的采样率的基础上的，不是目标音频帧的采样率)
   *  @note 一次的样本数不可能比uint32_t的最大值还大，所以这里返回值是uint32_t
   */
  uint32_t syncAud(uint32_t nbSamples, uint32_t freq);

  /*
   * @brief 用于判断是否需要设置好重采样器
   * @param fa 本次解码的音频帧
   * @param idealNbSamples 本次解码的音频帧的理想大小
   * @return 是否需要设置好重采样器
   * @note 以下两种情况需要设置好重采样器：
   * 1. 在本次视频帧之前，可能也已经有一些与Tgt不同的音频帧，但是最近的那个不同得情况也已经准备好对策了（就是重采样器已经设置好了），但是这次的音频帧与最近的那个不同得情况不同
   * 2. 本次的音频帧与最近的那个不同得情况相同，但是idealNbSamples与本视频帧的音频帧的样本数不同，我们需要减少或者增加样本数，但是这中操作也是需要在一个重采样器上进行的,
   *    此时如果连一个重采样器都没有，那么就需要设置好重采样器
   */
  [[nodiscard]] inline bool needPrepareSwrWithFmt(const Frame& fa, uint32_t idealNbSamples) const;
  inline void updateAudRecord(const Frame& fa);
};


inline SDLAudioDecodeRegin::SDLAudioDecodeRegin(
  FrameQueue& aFrameq,
  const SDLPlayState& playState,
  double tooBigDiffNoSync,
  uint16_t numDiffSamplesToSync,
  double weightOfOldestDiff
  ) :
  aFrameq(aFrameq),
  playState(playState),
  tooBigDiffNoSync(tooBigDiffNoSync),
  timeDiff(numDiffSamplesToSync,weightOfOldestDiff){
}

inline void SDLAudioDecodeRegin::setFromDisplayDecision(
  const AudioParamsDTO& audioParamsDTO,
  uint32_t hardwareAudBufSize
  ) {
  audTgtParams = audioParamsDTO; // 只能被设置一次，之后一点点都不能改变
  audRecordedParams.setShallowCopy(audTgtParams); // 这里因为audTgtParams被Set之后就不会再变了，所以这里可以直接浅拷贝
  this->hardwareAudBufSize = hardwareAudBufSize;
  diffThreshToAdjust = static_cast<double>(hardwareAudBufSize) / audTgtParams.getBytePerSec();
}

inline bool SDLAudioDecodeRegin::needPrepareSwrWithFmt(const Frame& fa, uint32_t idealNbSamples) const {
  return audRecordedParams.equalTo(
    fa.frame->sample_rate,
    &fa.frame->ch_layout,
    static_cast<AVSampleFormat>(fa.frame->format) // 这里需要将int转换为AVSampleFormat，Frame一定要是音频帧，这个保证需要上游（将Frame放进queue的地方）保证
  ) || (idealNbSamples != fa.frame->nb_samples && !swrCtx);
}

inline void SDLAudioDecodeRegin::updateAudRecord(const Frame& fa) {
  audRecordedParams.setDeepCopy(fa.frame->sample_rate, fa.frame->ch_layout, static_cast<AVSampleFormat>(fa.frame->format));
}

#endif //SDL_AUDIO_DECODE_REGIN_H
