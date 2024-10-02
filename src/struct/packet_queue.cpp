//
// Created by leo on 24-9-29.
//

#include "struct/packet_queue.h"

PacketQueue::PacketQueue() = default;

void PacketQueue::push(Packet&& pac) {
  std::unique_lock<std::mutex> lock(mtx);
  duration += pac.pktPtr->duration;
  buf.emplace_back(std::forward<Packet>(pac)); // 完美转发
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待pop
}

Packet PacketQueue::blockPop() {
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [this] { return !isEmpty();});
  Packet pkt = std::move(buf.front());
  buf.pop_front();
  duration -= pkt.pktPtr->duration;
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待push， 但是那个线程也要等待此函数返回（因为返回时才会unlock锁）
  return pkt; // 不需要std::move，因为返回值是个临时对象
}


std::optional<Packet> PacketQueue::tryPop() {
  if (isEmpty()) return std::nullopt;
  std::unique_lock<std::mutex> lock(mtx);
  if (isEmpty()) return std::nullopt; // 为什么要再次检查？因为可能在等待锁的时候，队列已经空了
  Packet pkt = std::move(buf.front());
  buf.pop_front();
  duration -= pkt.pktPtr->duration;
  cv.notify_all(); // 为什么通知？因为有可能有线程在等待push
  return pkt;
}

PacketQueue::~PacketQueue() {
  // 释放所有 pkt
  buf.clear(); // 释放pkt时会调用每个元素的析构函数，即释放AVPacket
}




