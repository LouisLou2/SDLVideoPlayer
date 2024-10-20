//
// Created by leo on 24-10-20.
//

#ifndef DIFF_TIME_H
#define DIFF_TIME_H
#include <cassert>
#include <cstdint>
#include <cmath>

class TimeDiff {
  /*-------------这两个是不变的-----------------*/
  const uint16_t numDiffNumToSync; // 用于求加权平均数的diff的个数
  const double oldestWeight; // 最老的diff的权值(注意是权值不是权重)
  const double weightQ; // 相当于等比数列的q
  /*-------------随运算变化的-----------------*/
  uint16_t numDiff; // 当前的diff个数
  double weightedDiff; // 乘以权值后的diff总和
  // double weightedMeanDiff; // weightedDiff除以权值总和的结果，是真正的加权平均数

public:
  inline TimeDiff(uint16_t numDiffNumToSync, double oldestWeight);
  /*
   * @brief 添加一个diff
   */
  inline void addDiff(double diff);
  /*
   * @brief 获取是不是达到了加权平均数的条件：即diff的个数是否达到了numDiffNumToSync
   * @return true表示达到了，false表示没有达到
   */
  [[nodiscard]] inline bool full() const;

  inline double getAndReset();

  inline void reset();
};

inline TimeDiff::TimeDiff(uint16_t numDiffNumToSync, double oldestWeight):
  numDiffNumToSync(numDiffNumToSync),
  oldestWeight(oldestWeight),
  weightedDiff(0), // 必须初始化为0
  numDiff(0), // 必须初始化为0
  weightQ(std::pow(oldestWeight, 1.0 / numDiffNumToSync - 1)){
  assert(oldestWeight > 0);
}

inline bool TimeDiff::full() const{
  return numDiff == numDiffNumToSync;
}

inline double TimeDiff::getAndReset() {
  /*其实算下来，到这一步，weightedDiff已经是每个都乘了权值的了，即weightedDiff = (weightQ^(numDiffNumToSync-1))*diff1 + (weightQ^(numDiffNumToSync-2))*diff2 + ... + (weightQ^0)*diff20,
   * 接下来就是除以权值总和，即(weightQ^(numDiffNumToSync-1) + weightQ^(numDiffNumToSync-2) + ... + weightQ^0)，
   * 所以这个权值总和是一个等比数列的和，可以用等比数列的求和公式求出来，即1*(1-std::pow(weightQ,numDiffNumToSync-1))/(1-weightQ)
   * 但是注意到之前要求了（强制断言oldestWeight = std::pow(weightQ,numDiffNumToSync-1)很小)，所以1*(1-std::pow(weightQ,numDiffNumToSync-1))视作1
   * 由此简化:
  */
  double ret = weightedDiff * (1 - weightQ);
  weightedDiff = 0;
  numDiff = 0;
  return ret;
}

inline void TimeDiff::addDiff(double diff){
  assert(numDiff < numDiffNumToSync);
  weightedDiff += diff * weightedDiff * weightQ;
}

inline void TimeDiff::reset(){
  weightedDiff = 0;
  numDiff = 0;
}

#endif //DIFF_TIME_H
