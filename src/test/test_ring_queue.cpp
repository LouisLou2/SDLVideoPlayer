// #include "struct/frame_queue.h"
// #include <iostream>
// #include <thread>
// #include <optional>
// #include <cassert>     // 假设 Packet 和 AVPacket 的定义也可用
// #include "struct/packet_queue.h"
//
//
// // 测试 PacketQueue 的初始化
// void testInitialization() {
//     std::cout << "Test Initialization..." << std::endl;
//     PacketQueue queue;
//     assert(queue.isEmpty());          // 队列应为空
//     assert(queue.getDuration() == 0); // 初始 duration 应为 0
//     assert(queue.getSerial() == 0);   // 初始 serial 应为 0
//     assert(queue.getPktCount() == 0); // 初始队列大小应为 0
//     std::cout << "Test Initialization Passed!" << std::endl;
// }
//
// // 测试 push 和 blockPop
// void testPushAndBlockPop() {
//     std::cout << "Test Push and BlockPop..." << std::endl;
//     PacketQueue queue;
//
//     // 创建模拟的 AVPacket
//     AVPacket* pkt1 = av_packet_alloc();
//     pkt1->duration = 100;
//
//     queue.push(pkt1);                  // 推入队列
//     assert(!queue.isEmpty());           // 队列不应为空
//     assert(queue.getPktCount() == 1);   // 队列大小应为 1
//
//     Packet poppedPacket = queue.blockPop();  // 从队列中弹出数据
//     assert(queue.isEmpty());                 // 弹出后队列应为空
//     assert(poppedPacket.pkt->duration == 100); // 检查弹出数据内容
//
//     std::cout << "Test Push and BlockPop Passed!" << std::endl;
// }
//
// // 测试 tryPop 操作
// void testTryPop() {
//     std::cout << "Test TryPop..." << std::endl;
//     PacketQueue queue;
//
//     // 空队列中弹出应返回空值
//     auto optPacket = queue.tryPop();
//     assert(!optPacket.has_value());  // 空队列应返回 std::nullopt
//
//     // 推入一个 AVPacket
//     AVPacket* pkt2 = av_packet_alloc();
//     pkt2->duration = 200;
//     queue.push(pkt2);
//
//     // 非空队列中应返回 Packet
//     optPacket = queue.tryPop();
//     assert(optPacket.has_value());               // 队列非空，返回 Packet
//     assert(optPacket->pkt->duration == 200);     // 检查数据内容
//
//     // 再次尝试从空队列弹出
//     optPacket = queue.tryPop();
//     assert(!optPacket.has_value());              // 再次为空应返回空
//
//     std::cout << "Test TryPop Passed!" << std::endl;
// }
//
// // 测试多线程下的同步
// void testMultiThreaded() {
//     std::cout << "Test MultiThreaded..." << std::endl;
//     PacketQueue queue;
//
//     // 创建模拟的 AVPacket
//     AVPacket* pkt3 = av_packet_alloc();
//     pkt3->duration = 300;
//
//     // 启动生产者线程
//     std::thread producer([&queue, &pkt3]() {
//         for (int i = 0; i < 1000; ++i) {
//             queue.push(pkt3);  // 推入数据
//         }
//     });
//
//     // 启动消费者线程
//     std::thread consumer([&queue]() {
//         for (int i = 0; i < 1000; ++i) {
//             Packet packet = queue.blockPop();  // 弹出数据
//             assert(packet.pkt->duration == 300);
//         }
//     });
//
//     // 等待线程完成
//     producer.join();
//     consumer.join();
//
//     assert(queue.isEmpty());  // 最终队列应为空
//     std::cout << "Test MultiThreaded Passed!" << std::endl;
// }
//
// // 主测试函数
// int main() {
//     testInitialization();
//     testPushAndBlockPop();
//     testTryPop();
//     testMultiThreaded();
//
//     std::cout << "All Tests Passed!" << std::endl;
//     return 0;
// }
