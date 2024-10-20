//
// Created by leo on 24-10-10.
//

#ifndef AUDIO_PARAMS_H
#define AUDIO_PARAMS_H
#include <cstdint>

#include "audio_param_dto.h"
#ifdef __cplusplus
extern "C" {
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
}
#else
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#endif

class AudioParams {
  // friend class SDLMediaFilterInfo;
  // friend class SDLVideoPlayer;
  uint32_t freq;
  AVChannelLayout ch_layout;
  AVSampleFormat fmt;
  /*
   * 一个音频帧的大小，单位是字节
   * 这里定义他就是所有channel都得到一个fmt的一个样本的大小
   */
  uint32_t frameSize;
  /*
   * bytePerSec = frameSize * framePerSec
   * 而这里的framePerSec其实就是freq
   */
  uint32_t bytePerSec;
public:
  AudioParams() = default;
  /*
   * @brief 从AudioParamsDTO构造
   * @param dto AudioParamsDTO
   * @note ch_layout指针指向的资源我们不关心他的归属，我们只是复制他的内容
   */
  explicit AudioParams(const AudioParamsDTO& ap) = delete;
  explicit AudioParams(const AudioParams& ap) = delete;
  AudioParams& operator=(const AudioParamsDTO& dto);
  void shallowCopy(const AudioParams& dto);
  // setter
  void setFreq(uint32_t freq) {this->freq = freq;}
  void copySetChLayout(const AVChannelLayout* ch_layout) { av_channel_layout_copy(&this->ch_layout, ch_layout); }
  void setFmt(AVSampleFormat fmt) {this->fmt = fmt;}
  /*
   * @brief set the audio params
   * @note the ch_layout 这个指针指向的资源我们不关心他的归属，我们只是复制他的内容
   */
  void setParams(uint32_t freq, const AVChannelLayout* ch_layout, AVSampleFormat fmt);
  // getters: all inline
  [[nodiscard]] uint32_t getFreq() const {return freq;}
  [[nodiscard]] const AVChannelLayout* getChLayoutPtr() const {return &ch_layout;}
  [[nodiscard]] AVSampleFormat getFmt() const {return fmt;}
  [[nodiscard]] uint32_t getFrameSize() const {return frameSize;}
  [[nodiscard]] uint32_t getBytePerSec() const {return bytePerSec;}
};

#endif //AUDIO_PARAMS_H
