//
// Created by leo on 24-9-29.
//

#ifndef PACKET_H
#define PACKET_H
#include <iostream>
#include <ostream>

#ifdef __cplusplus
extern "C"{
#include <libavcodec/packet.h>
}
#else
#include <libavcodec/packet.h>
#endif

struct Packet {
  AVPacket *pkt = nullptr;
  uint32_t serial;

  Packet() = default;
  Packet(AVPacket *pkt, uint32_t serial);

  Packet(const Packet&) = default;
  Packet& operator=(const Packet&) = default;

  // move constructor
  Packet(Packet&& rhs) noexcept;
  Packet& operator=(Packet&& rhs) noexcept;

  // ==
  bool operator==(const Packet& rhs) const;

  void releaseAndReset(AVPacket *pkt, uint32_t serial);
  void release();
  // 注意这里不加析构，加了会导致PacketQueue不正常的行为
  ~Packet() = default;
};

inline Packet::Packet(AVPacket *pkt, uint32_t serial) : pkt(pkt), serial(serial) {}

inline Packet::Packet(Packet&& rhs) noexcept : pkt(rhs.pkt), serial(rhs.serial) {
  rhs.pkt = nullptr;
}

inline Packet& Packet::operator=(Packet&& rhs) noexcept {
  if (this == &rhs) return *this;
  if (pkt) av_packet_free(&pkt);
  pkt = rhs.pkt;
  serial = rhs.serial;
  rhs.pkt = nullptr;
  return *this;
}

inline bool Packet::operator==(const Packet& rhs) const {
  return pkt == rhs.pkt && serial == rhs.serial;
}

inline void Packet::releaseAndReset(AVPacket* pkt, uint32_t serial) {
  if (this->pkt) {
    av_packet_free(&this->pkt);
  }
  this->pkt = pkt;
  this->serial = serial;
}

inline void Packet::release() {
  if (pkt) av_packet_free(&pkt);
}

#endif //PACKET_H
