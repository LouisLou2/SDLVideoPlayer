//
// Created by leo on 24-10-15.
//

#ifndef SDL_AUDIO_DECODE_REGIN_H
#define SDL_AUDIO_DECODE_REGIN_H

#include <const/show_mode_enum.h>

#include "sdl_play_state.h"
#include "sdl_presentation_form.h"
#include "struct/frame_queue.h"
#include "entity/audio_params.h"

class SDLAudioDecodeRegin {

  uint64_t callback_time;
  uint64_t hardwareAudBufSize;
  // 音频的源参数和目标参数
  AudioParams audSrcParams;
  AudioParams audTgtParams;
  uint32_t sdlAudioBufThreshold;
  FrameQueue& aFrameq;
  // 下面是用于音视频播放控制的变量，他们集结在SDLVideoPlayer的PlayState中，这里我们引用他(const形式)
  // TODO: 实际上不是其中的所有值都会使用到，这样直接引用会导致不必要的耦合，后续可以考虑将其中的一部分值提取出来
  const SDLPlayState& playState;
public:
  // cons
  SDLAudioDecodeRegin(
    FrameQueue& aFrameq,
    const SDLPlayState& playState
  );

  int decodeFrame();

  /*
   *  @brief 用于音频同步
   *  @param nbSamples 本次解码的音频帧的样本数
   *  @return 为了达到同步目的，理想的音频帧数应该是多少
   *  @note 一次的样本数不可能比uint32_t的最大值还大，所以这里返回值是uint32_t
   */
  uint32_t syncAud(uint32_t nbSamples);
};


inline SDLAudioDecodeRegin::SDLAudioDecodeRegin(
  FrameQueue& aFrameq,
  const SDLPlayState& playState
) :
aFrameq(aFrameq),
playState(playState) {

}
#endif //SDL_AUDIO_DECODE_REGIN_H
