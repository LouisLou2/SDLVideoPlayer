//
// Created by leo on 24-9-29.
//

#include "struct/packet_queue.h"

PacketQueue::PacketQueue() = default;

void PacketQueue::push(AVPacket* pkt) {
  std::unique_lock<std::mutex> lock(mtx);
  buf.emplace_back(pkt, serial);
  duration += pkt->duration;
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待pop
}

Packet PacketQueue::blockPop() {
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [this] { return !isEmpty();});
  Packet pkt = buf.front();
  buf.pop_front();
  duration -= pkt.pkt->duration;
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待push
  return pkt;
}

std::optional<Packet> PacketQueue::tryPop() {
  if (isEmpty()) return std::nullopt;
  std::unique_lock<std::mutex> lock(mtx);
  if (isEmpty()) return std::nullopt; // 为什么要再次检查？因为可能在等待锁的时候，队列已经空了
  Packet pkt = buf.front();
  buf.pop_front();
  duration -= pkt.pkt->duration;
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待push
  return pkt;
}

PacketQueue::~PacketQueue() {
  // 释放所有 pkt
  for (auto &pkt : buf) {
    pkt.release();
  }
}




