//
// Created by leo on 24-10-6.
//

#ifndef CALC_H
#define CALC_H
#include <cstdint>

class Calc {
public:
  inline static int32_t clip(int32_t val, int32_t min, int32_t max);
};

inline int32_t Calc::clip(int32_t val, int32_t min, int32_t max) {
  return val < min ? min : (val > max ? max : val);
}

#endif //CALC_H
