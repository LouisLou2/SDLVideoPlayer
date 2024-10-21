//
// Created by leo on 24-9-29.
//

#ifndef FRAME_H
#define FRAME_H
#include "subtitle.h"
#ifdef __cplusplus
extern "C"{
#include <libavutil/frame.h>
}
#else
#include <libavutil/frame.h>
#endif

struct Frame {
  AVFrame* frame = nullptr;
  Subtitle subt{};
  uint32_t serial = 0;
  double pts = 0;
  double duration = 0;
  uint64_t pos = 0; // bytepos in input stream
  uint32_t width = 0;
  uint32_t height = 0;
  int32_t fmt{};// ffmpeg的格式太多，先用ffmpeg的enum吧， enum class之后再说
  AVRational sar{};// 宽高比
  bool uploaded = false;
  bool flip = false;
  // static:
  static Frame getCopy(const Frame& frame);
  // non-static
  explicit Frame() = default;
  //TODO： 这里subt的构造采用的使用Subtitle的默认拷贝构造，之后看看是不是可以修改更加合理，现在先不改了
  explicit Frame(AVFrame* frame, const Subtitle& subt, uint32_t serial, double pts, double duration, uint64_t pos, uint32_t width, uint32_t height, int32_t fmt, AVRational sar, bool uploaded, bool flip);
  // 使用复制构造函数一定要慎重再慎重,这里将复制构造函数删除，但是定义一个具有名字的函数，为了防止编码的时候无意识的复制
  Frame(const Frame&) = delete;
  Frame &operator=(const Frame&) = delete;
  void shallowCopy(const Frame& fr);
  // move constructor
  Frame(Frame&& rhs) noexcept;
  Frame& operator=(Frame&& rhs) noexcept;
  // release frame
  void release();
  // release and reset
  void releaseAndReset(const Frame& fr);
  ~Frame() {
    if (frame) av_frame_free(&frame);
  }
};


inline Frame::Frame(Frame&& rhs) noexcept : frame(rhs.frame),
serial(rhs.serial), pts(rhs.pts), duration(rhs.duration),
pos(rhs.pos), width(rhs.width), height(rhs.height),
fmt(rhs.fmt), sar(rhs.sar), uploaded(rhs.uploaded),
flip(rhs.flip), subt(rhs.subt) {
  rhs.frame = nullptr;
}

inline void Frame::release() {
  if (frame) av_frame_free(&frame);
}

#endif //FRAME_H
