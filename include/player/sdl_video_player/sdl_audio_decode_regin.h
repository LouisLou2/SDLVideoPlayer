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
  //

  // 音频的源参数和目标参数
  AudioParams audSrcParams;
  AudioParams audTgtParams;

  uint32_t sdlAudioBufThreshold;
  FrameQueue aFrameq;
  // 下面是用于音视频播放控制的变量，他们集结在SDLVideoPlayer的PlayState中，这里我们引用他(const形式)
  // TODO: 实际上不是其中的所有值都会使用到，这样直接引用会导致不必要的耦合，后续可以考虑将其中的一部分值提取出来
  const SDLPlayState& playState;

public:
  // cons
  SDLAudioDecodeRegin(
      size_t aFrameqSize,
      bool aFrameqKeepLast,
      const SDLPlayState& playState
    );
  void decodeFrame();
};


inline SDLAudioDecodeRegin::SDLAudioDecodeRegin(
    size_t aFrameqSize,
    bool aFrameqKeepLast,
    const SDLPlayState& playState
  ) :
  aFrameq(aFrameqSize, aFrameqKeepLast),
  playState(playState){}
#endif //SDL_AUDIO_DECODE_REGIN_H
