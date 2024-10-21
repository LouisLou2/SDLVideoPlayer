//
// Created by leo on 24-10-21.
//

#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H
#include <cassert>
#include <cstdint>

class MemAlloc {
public:
  /*
   * @brief 尝试重用内存，如果内存不够，就重新分配
   * 如果重新分配，ptr和size会被修改
   * @此函数不检查ptr是否为nullptr，只以size是否小于newMinSize为准,所以调用者需要保证二者的一致性
   */
  static inline void tryReuse(uint8_t*& ptr, uint32_t& size, uint32_t newMinSize);
};

inline void MemAlloc::tryReuse(uint8_t*& ptr, uint32_t& size, uint32_t newMinSize) {
  assert(ptr && size > 0 || !ptr && size == 0);
  if (size < newMinSize) {
    delete[] ptr;
    ptr = new uint8_t[newMinSize];
    size = newMinSize;
  }
}

#endif //MEM_ALLOC_H
