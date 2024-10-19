//
// Created by leo on 24-10-15.
//

#ifndef SDL_VIDEO_DECODE_REGIN_H
#define SDL_VIDEO_DECODE_REGIN_H

#include "struct/frame_queue.h"
#include "sdl_play_state.h"

class SDLVideoDecodeRegin {
  FrameQueue& vFrameq;
  FrameQueue& sFrameq;

  // 下面是用于音视频播放控制的变量，他们集结在SDLVideoPlayer的PlayState中，这里我们引用他(const形式)
  // TODO: 实际上不是其中的所有值都会使用到，这样直接引用会导致不必要的耦合，后续可以考虑将其中的一部分值提取出来
  const SDLPlayState& playState;

public:
  // cons
  SDLVideoDecodeRegin(FrameQueue& vFrameqRef,FrameQueue& sFrameqRef, const SDLPlayState& playState);
};

inline SDLVideoDecodeRegin::SDLVideoDecodeRegin(FrameQueue& vFrameqRef,FrameQueue& sFrameqRef, const SDLPlayState& playState):
  vFrameq(vFrameqRef),
  sFrameq(sFrameqRef),
  playState(playState){
}
#endif //SDL_VIDEO_DECODE_REGIN_H
