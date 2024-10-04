//
// Created by leo on 24-9-29.
//
#pragma once

#define PACKET_QUEUE_SIZE 1024

#include <array>
#include <condition_variable>
#include <mutex>
#include <optional>

#include "entity/frame.h"
#ifdef __cplusplus
extern "C"{
#include <libavformat/avformat.h>
}
#else
#endif


// 这个队列基本上就是单生产者单消费者
class FrameQueue {
  // 内部数据结构：ring buffer
  uint16_t capacity = PACKET_QUEUE_SIZE - 1; // 队列中留一个空位，用于区分队列满和队列空
  int16_t bufCap = PACKET_QUEUE_SIZE;
  std::array<Frame,PACKET_QUEUE_SIZE> buf;
  uint16_t head = 0; // head指向下一个可读位置
  uint16_t tail = 0; // tail指向下一个可写位置

  // 读写同步
  std::mutex mtx;
  std::condition_variable cv;

public:
  explicit FrameQueue() = default;
  [[nodiscard]] inline uint16_t getCapacity() const;
  [[nodiscard]] inline uint32_t getPktCount() const;
  [[nodiscard]] bool isFull() const;
  [[nodiscard]] bool isEmpty() const;

  /*
   * BEWARE: 向其中推送同一内存地址的Frame，会导致内存重复释放
   */
  void blockPush(Frame& fr);
  bool tryPush(Frame& fr);
  Frame blockPop();
  std::optional<Frame> tryPop();

  ~FrameQueue(); // Frame的析构函数会释放pkt
};

inline uint16_t FrameQueue::getCapacity() const {
  return capacity;
}

inline uint32_t FrameQueue::getPktCount() const {
  return (tail + bufCap - head) % bufCap;
}

inline bool FrameQueue::isFull() const {
  return (tail + 1) % bufCap == head;
}

inline bool FrameQueue::isEmpty() const {
  return head == tail;
}
