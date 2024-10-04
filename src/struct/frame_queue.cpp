//
// Created by leo on 24-9-29.
//

#include "struct/frame_queue.h"

void FrameQueue::blockPush(Frame& fr) {
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [this] { return !isFull();});
  buf[tail].releaseAndReset(fr);
  tail = (tail + 1) % bufCap;
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待pop
}

bool FrameQueue::tryPush(Frame& fr) {
  if (isFull()) return false;
  std::unique_lock<std::mutex> lock(mtx);
  if (isFull()) return false; // 为什么要再次检查？因为可能在等待锁的时候，队列已经满了
  buf[tail].releaseAndReset(fr);
  tail = (tail + 1) % bufCap;
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待pop
  return true;
}

Frame FrameQueue::blockPop() {
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [this] { return !isEmpty();});
  Frame pkt = std::move(buf[head]);
  head = (head + 1) % bufCap;
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待push
  return pkt;
}

std::optional<Frame> FrameQueue::tryPop() {
  if (isEmpty()) return std::nullopt;
  std::unique_lock<std::mutex> lock(mtx);
  if (isEmpty()) return std::nullopt; // 为什么要再次检查？因为可能在等待锁的时候，队列已经空了
  Frame pkt = std::move(buf[head]);
  head = (head + 1) % bufCap;
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待push
  return pkt;
}

FrameQueue::~FrameQueue() {
  // 释放所有 pkt
  for (int i = head; i != tail; i = (i + 1) % bufCap) {
    buf[i].release();
  }
}