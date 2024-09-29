//
// Created by leo on 24-9-29.
//

#include "struct/packet_queue.h"
#include "util/atomic_queue/atomic_queue.h"

void testLibQueue() {
  atomic_queue::AtomicQueue<int, 10, -1, true, true, false> q;
  int count = 0;
  for (int i = 0; i < 5; ++i) {
    if (q.try_push(i)) {
      ++count;
    } else {
      break;
    }
  }
  // pushed num
  std::cout << "pushed num:" << count << std::endl;
  std::cout << "capicity:"<<q.capacity()<< std::endl;
  std::cout << "------------------" << std::endl;

  for (int i = 0; i < count; ++i) {
    int v;
    if (q.try_pop(v)) {
      std::cout << v << std::endl;
    }
  }
  for (int i = 0; i < 10000; ++i) {
    q.push(i); // blocking push
  }
}