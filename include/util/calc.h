//
// Created by leo on 24-10-6.
//

#ifndef CALC_H
#define CALC_H
#include <cstdint>

class Calc {
public:
  static inline constexpr uint8_t max(uint8_t a, uint8_t b, uint8_t c);
  // inline static int32_t clip(int32_t val, int32_t min, int32_t max);
};

inline constexpr uint8_t Calc::max(uint8_t a, uint8_t b, uint8_t c) {
  return a > b ? (a > c ? a : c) : (b > c ? b : c);
}

#endif //CALC_H