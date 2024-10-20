//
// Created by leo on 24-9-29.
//

#include "struct/frame_queue.h"

void FrameQueue::reset(size_t capacity, bool keepLast) {
  assert(isEmpty());
  // 检查capacity是否合法
  assert(capacity <= CapMax);
  // 重置
  this->capacity = static_cast<uint16_t>(capacity);
  bufCap = this->capacity + 1;
  this->keepLast = keepLast;
}

void FrameQueue::blockPush(Frame&& fr) {
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [this] { return !isFull();});
  buf[tail] = std::move(fr);
  tail = (tail + 1) % bufCap;
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待pop
}

bool FrameQueue::tryPush(Frame&& fr) {
  if (isFull()) return false;
  std::unique_lock<std::mutex> lock(mtx);
  if (isFull()) return false; // 为什么要再次检查？因为可能在等待锁的时候，队列已经满了
  buf[tail] = std::move(fr);
  tail = (tail + 1) % bufCap;
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待pop
  return true;
}

Frame FrameQueue::blockPop() {
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [this] { return !isEmpty();});
  // 如果是keepLast，那么需要复制一份
  if (keepLast) {
    // 正好循环queue的最后一个位置不是内容，利用它存储lastFrame
    buf[tail].shallowCopy(buf[head]);
    /*此处特别注意的是，pop出去的一个Frame，这里还会保存一个Frame，
     *他们的内部指针指向的是同一块内存，所以使用keepLast机制需要格外注意！！
     */
  }
  Frame pkt = std::move(buf[head]);
  head = (head + 1) % bufCap;
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待push
  return pkt;
}

std::optional<Frame> FrameQueue::tryPop() {
  if (isEmpty()) return std::nullopt;
  std::unique_lock<std::mutex> lock(mtx);
  if (isEmpty()) return std::nullopt; // 为什么要再次检查？因为可能在等待锁的时候，队列已经空了
  // 如果是keepLast，那么需要复制一份
  if (keepLast) {
    // 正好循环queue的最后一个位置不是内容，利用它存储lastFrame
    buf[tail].shallowCopy(buf[head]);
    /*此处特别注意的是，pop出去的一个Frame，这里还会保存一个Frame，
     *他们的内部指针指向的是同一块内存，所以使用keepLast机制需要格外注意！！
     */
  }
  Frame pkt = std::move(buf[head]);
  head = (head + 1) % bufCap;
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待push
  return pkt;
}

Frame FrameQueue::getLastFrame() {
  assert(keepLast);
  // 需要拿到锁，防止pop的时候修改lastFrame
  std::unique_lock<std::mutex> lock(mtx);
  // 在queue的尾部
  return std::move(buf[tail]);
}

FrameQueue::~FrameQueue() {
  // 释放所有 pkt
  for (size_t i = head; i != tail; i = (i + 1) % bufCap) {
    buf[i].release();
  }
  // 释放lastFrame
  if (keepLast)
    buf[tail].release();
}
