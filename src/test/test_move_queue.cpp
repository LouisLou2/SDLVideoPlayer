// #include <iostream>
// #include <thread>
// #include <chrono>
// #include <cassert>
// #include "struct/packet_queue.h"  // 包含你的 PacketQueue 头文件
// extern "C"{
// #include <libavformat/avformat.h>
// }
//
// // 生产者函数，向队列中推入指定数量的 Packet
// void producer(PacketQueue &queue, int numPackets) {
//   for (int i = 0; i < numPackets; ++i) {
//     // 创建 Packet，并给 pktPtr 分配资源
//     AVPacket* pkt = av_packet_alloc();
//     Packet packet(pkt, i);
//     packet.pktPtr->duration = 10 + i;  // 假设每个包的 duration 是不同的
//
//     // 将 packet 推入队列
//     queue.push(std::move(packet));
//
//     // 检查 packet 资源是否已转移，packet 应该已经失去 pktPtr 的所有权
//     assert(packet.pktPtr == nullptr && "Packet's resources were not moved correctly!");
//
//     std::cout << "Producer pushed packet with duration: " << 10 + i << std::endl;
//   }
// }
//
// // 消费者函数，从队列中阻塞式弹出 Packet
// void consumer(PacketQueue &queue, int numPackets) {
//   for (int i = 0; i < numPackets; ++i) {
//     // 阻塞式弹出 packet
//     Packet packet = queue.blockPop();
//
//     // 检查 packet 是否成功弹出并持有资源
//     assert(packet.pktPtr != nullptr && "Consumer received an invalid packet!");
//
//     std::cout << "Consumer got packet with duration: " << packet.pktPtr->duration << std::endl;
//   }
// }
//
// int main() {
//   PacketQueue queue;
//
//   int numPackets = 1000;  // 要测试的 packet 数量
//
//   // 创建两个线程：一个生产者和一个消费者
//   std::thread prodThread(producer, std::ref(queue), numPackets);  // 生产 numPackets 个 packet
//   std::thread consThread(consumer, std::ref(queue), numPackets);  // 消费 numPackets 个 packet
//
//   // 等待所有线程完成
//   prodThread.join();
//   consThread.join();
//
//   std::cout << "Producer and Consumer threads finished." << std::endl;
//   return 0;
// }