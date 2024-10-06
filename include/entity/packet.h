//
// Created by leo on 24-9-29.
//

#ifndef PACKET_H
#define PACKET_H
#include <memory>
#ifdef __cplusplus
extern "C"{
#include <libavcodec/packet.h>
}
#else
#include <libavcodec/packet.h>
#endif

#include "util/mem/ff_mem.h"

struct Packet {
  std::unique_ptr<AVPacket,AVPacketDeleter> pktPtr = nullptr;
  uint32_t serial;

  Packet(AVPacket *pkt, uint32_t serial);
  // 禁止复制构造
  Packet(const Packet&) = delete;
  Packet& operator=(const Packet&) = delete;
  // 移动构造
  Packet(Packet&& rhs) noexcept;
  Packet& operator=(Packet&& rhs) noexcept;

  void releaseAndReset(AVPacket *pkt, uint32_t serial);
  void release();
  // 注意这里不加析构，加了会导致PacketQueue不正常的行为
  ~Packet();
};

/*
 * 建立对象后，原始的avpacket指针就再外部不要再使用了
 */
inline Packet::Packet(AVPacket *pkt, uint32_t serial) : pktPtr(pkt), serial(serial) {}

inline Packet::Packet(Packet&& rhs) noexcept:
pktPtr(std::move(rhs.pktPtr)),
serial(rhs.serial) {
  rhs.pktPtr = nullptr;
}

inline Packet& Packet::operator= (Packet&& rhs) noexcept {
  if (this == &rhs) return *this;
  pktPtr.reset(); // 释放自己的pkt, 如果管理的指针是空的，那么reset不会有任何效果
  pktPtr = std::move(rhs.pktPtr); // 同时rhs的pktPtr会被置为nullptr
  serial = rhs.serial;
  return *this;
}


inline void Packet::releaseAndReset(AVPacket* pkt, uint32_t serial) {
  pktPtr.reset(pkt);
  this->serial = serial;
}

inline void Packet::release() {
  pktPtr.reset();
}

inline Packet::~Packet() {
  pktPtr.reset();
}

#endif //PACKET_H
