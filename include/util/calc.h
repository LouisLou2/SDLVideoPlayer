//
// Created by leo on 24-10-6.
//

#ifndef CALC_H
#define CALC_H

#include <cstdint>
#include <cassert>
#include <cmath>

class Calc {
public:
  static constexpr uint8_t max(uint8_t a, uint8_t b, uint8_t c);
  static constexpr double getNthRoot(uint16_t n, double y); // 目前定义它没啥用，还是用std::pow
  static constexpr uint32_t getCorSampleNum(uint32_t sampleNum, uint32_t sampleRate, uint32_t tgtSampleRate);
};

constexpr uint8_t Calc::max(uint8_t a, uint8_t b, uint8_t c) {
  return a > b ? (a > c ? a : c) : (b > c ? b : c);
}

constexpr double Calc::getNthRoot(uint16_t n, double y) {
  assert(y>0 || (y < 0 && n%2==1));
  // 尝试使用指数对数法
  return exp(std::log(y>0 ? y : -y) / n) * (y > 0 ? 1 : -1);
}

constexpr uint32_t Calc::getCorSampleNum(uint32_t sampleNum, uint32_t sampleRate, uint32_t tgtSampleRate) {
  return sampleNum * tgtSampleRate / sampleRate;
}

#endif //CALC_H