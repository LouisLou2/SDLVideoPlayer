//
// Created by leo on 24-9-29.
//
#pragma once

#include <condition_variable>
#include <list>
#include <mutex>
#include <optional>

#include "entity/packet.h"
#ifdef __cplusplus
extern "C"{
#include <libavformat/avformat.h>
}
#else
#endif


// 这个队列基本上就是单生产者单消费者, AutoGrow
class PacketQueue {

  std::list<Packet> buf;
  // 读写同步
  std::mutex mtx;
  std::condition_variable cv;
  // 应用层数据
  uint64_t duration = 0; // unit: time_base
  uint32_t serial = 0;

public:
  PacketQueue();
  [[nodiscard]] inline uint64_t getDuration() const;
  [[nodiscard]] inline uint32_t getSerial() const;
  [[nodiscard]] inline uint32_t getPktCount() const;
  [[nodiscard]] bool isEmpty() const;
  /*
   * BEWARE: 向其中推送同一内存地址的Frame，会导致内存重复释放
   * push方法是阻塞的, push一个pkt, 不会影响原来的pkt，
   * 但是请不要对外部的pkt再进行修改了，它的使用权已经交给了PacketQueue
   */
  void push(Packet&& pac);
  /*
   * 一旦pop掉
   */
  Packet blockPop();
  std::optional<Packet> tryPop();
  ~PacketQueue(); // Packet的析构函数会释放pkt
};

inline uint64_t PacketQueue::getDuration() const {
  return duration;
}

inline uint32_t PacketQueue::getSerial() const {
  return serial;
}

inline uint32_t PacketQueue::getPktCount() const {
  return buf.size();
}

inline bool PacketQueue::isEmpty() const {
  return buf.empty();
}
