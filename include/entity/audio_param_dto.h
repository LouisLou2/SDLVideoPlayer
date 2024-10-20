//
// Created by leo on 24-10-20.
//

#ifndef AUDIO_PARAM_DTO_H
#define AUDIO_PARAM_DTO_H
#include <cstdint>
#ifdef __cplusplus
extern "C" {
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
}
#else
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#endif

/*
 * 解释一下此类的作用：
 * 1. 此类仅仅只是一个数据传输对象，代码中仅仅依赖AudioParams类
 * 2. 由于AVChannelLayout是C语言的结构体, 没有移动语义
 * 3. 为了数据某些情况下（即AVChannelLayout*资源稳定）的传递，我们需要一个类似的类，防止过多的复制
 * 4. 另一种做法是使用std::function直接传递要设置的函数，但是这样会导致代码的可读性变差，
 * 或者传递要设置变量的引用，但是这样侵入性太强所以我们选择了这种方式
 */
class AudioParamsDTO {
  friend class AudioParams;
  uint32_t freq;
  const AVChannelLayout* ch_layout;
  AVSampleFormat fmt;
  uint32_t frameSize;
  uint32_t bytePerSec;
public:
  AudioParamsDTO(
    uint32_t freq,
    const AVChannelLayout* ch_layout,
    AVSampleFormat fmt
  );
  // void setParams(uint32_t freq, const AVChannelLayout* ch_layout, AVSampleFormat fmt, uint32_t frameSize, uint32_t framePerSec);
};

#endif //AUDIO_PARAM_DTO_H
