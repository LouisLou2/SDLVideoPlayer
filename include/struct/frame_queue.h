//
// Created by leo on 24-9-29.
//
#pragma once

#define PIC_CAP_MAX 3
#define SAMPLE_CAP_MAX 9
#define SUB_CAP_MAX 16

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
  static constexpr uint16_t CapMax = std::max(PIC_CAP_MAX, std::max(SAMPLE_CAP_MAX, SUB_CAP_MAX));
  // 内部数据结构：ring buffer
  size_t capacity; // 队列中留一个空位，用于区分队列满和队列空
  size_t bufCap;
  std::array<Frame, CapMax + 1> buf; // 内部数据结构，始终分配最大的空间，不用担心空间浪费，因为最大的空间也才几个
  size_t head = 0; // head指向下一个可读位置
  size_t tail = 0; // tail指向下一个可写位置

  // 读写同步
  std::mutex mtx;
  std::condition_variable cv;

  // 自定义选项
  bool keepLast;
  Frame lastFrame;

public:
  inline explicit FrameQueue();
  inline explicit FrameQueue(size_t capacity, bool keepLast = false);
  // 此方法尽可以在队列为空的时候调用，实际上它仅仅准备在还未投入使用时使用
  void reset(size_t capacity, bool keepLast);
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
  Frame getLastFrame();
  ~FrameQueue(); // Frame的析构函数会释放pkt
};

inline FrameQueue::FrameQueue(): capacity(CapMax), bufCap(CapMax + 1), keepLast(false) {}

inline FrameQueue::FrameQueue(size_t capacity, bool keepLast): keepLast(keepLast) {
  // 检查capacity是否合法
  assert(capacity <= CapMax);
  this->capacity = static_cast<uint16_t>(capacity);
  bufCap = this->capacity + 1;
}

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
