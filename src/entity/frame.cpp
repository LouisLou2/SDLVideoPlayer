//
// Created by leo on 24-9-29.
//

#include "entity/frame.h"

Frame::Frame(
  AVFrame* frame,
  const Subtitle& subt,
  uint32_t serial,
  double pts,
  double duration,
  uint64_t pos,
  uint32_t width,
  uint32_t height,
  int32_t fmt,
  AVRational sar,
  bool uploaded,
  bool flip
  ): frame(frame), subt(subt), serial(serial), pts(pts),
  duration(duration), pos(pos), width(width),height(height),
  fmt(fmt), sar(sar), uploaded(uploaded), flip(flip) {}


Frame Frame::getCopy(const Frame& frame) {
  return Frame(frame.frame, frame.subt, frame.serial, frame.pts, frame.duration, frame.pos, frame.width, frame.height, frame.fmt, frame.sar, frame.uploaded, frame.flip);
}

void Frame::shallowCopy(const Frame& fr) {
  frame = fr.frame;
  serial = fr.serial;
  pts = fr.pts;
  duration = fr.duration;
  pos = fr.pos;
  width = fr.width;
  height = fr.height;
  fmt = fr.fmt;
  sar = fr.sar;
  uploaded = fr.uploaded;
  flip = fr.flip;
  subt = fr.subt;
}

Frame& Frame::operator=(Frame&& rhs) noexcept {
  if (this == &rhs) return *this; // 不做检查
  frame = rhs.frame;
  serial = rhs.serial;
  pts = rhs.pts;
  duration = rhs.duration;
  pos = rhs.pos;
  width = rhs.width;
  height = rhs.height;
  fmt = rhs.fmt;
  sar = rhs.sar;
  uploaded = rhs.uploaded;
  flip = rhs.flip;
  subt = rhs.subt;
  rhs.frame = nullptr;
  return *this;
}

void Frame::releaseAndReset(const Frame& fr) {
  if (frame) av_frame_free(&frame);
  frame = fr.frame;
  serial = fr.serial;
  pts = fr.pts;
  duration = fr.duration;
  pos = fr.pos;
  width = fr.width;
  height = fr.height;
  fmt = fr.fmt;
  sar = fr.sar;
  uploaded = fr.uploaded;
  flip = fr.flip;
  subt = fr.subt;
}
