//
// Created by leo on 24-9-29.
//

#ifndef FRAME_H
#define FRAME_H
#ifdef __cplusplus
extern "C"{
#include <libavutil/frame.h>
}
#else
#include <libavutil/frame.h>
#endif

struct Frame {
  AVFrame* frame = nullptr;
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

  Frame() = default;
  Frame(AVFrame* frame, uint32_t serial, double pts, double duration, uint64_t pos, uint32_t width, uint32_t height, int32_t fmt, AVRational sar, bool uploaded, bool flip);
  // 删除复制构造
  Frame(const Frame&) = delete;
  Frame &operator=(const Frame&) = delete;
  // move constructor
  Frame(Frame&& rhs) noexcept;
  // Frame& operator=(Frame&& rhs) noexcept;
  // release frame
  void release();
  // release and reset
  void releaseAndReset(const Frame& fr);
  // releaseAndReset
  ~Frame() = default;
};


inline Frame::Frame(Frame&& rhs) noexcept : frame(rhs.frame),
serial(rhs.serial), pts(rhs.pts), duration(rhs.duration),
pos(rhs.pos), width(rhs.width), height(rhs.height),
fmt(rhs.fmt), sar(rhs.sar), uploaded(rhs.uploaded),
flip(rhs.flip) {
  rhs.frame = nullptr;
}

inline void Frame::release() {
  if (frame) av_frame_free(&frame);
}

#endif //FRAME_H
