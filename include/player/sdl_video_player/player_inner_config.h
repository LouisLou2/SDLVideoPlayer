//
// Created by leo on 24-10-20.
//

#ifndef PLAYER_INNER_CONFIG_H
#define PLAYER_INNER_CONFIG_H
#include <cstdint>
#include <cassert>
/*
 * @brief 用于存放播放器内部的配置, 这些配置不暴露给调用者
 */
class PlayerInnerConfig {
  friend class SDLVideoPlayer;
private:
  // 太大的时间差，不进行同步,怀疑是因为文件pts有问题
  const double tooBigDiffNoSync;
  // 用于音频同步分析的diff次数，实际中是需要多个diff进行加权，来分析同步情况
  const uint16_t numDiffSamplesToSync;
  /*
   * @brief 用于音频同步分析的加权，这个值是最老的diff的权值
   * @example 例如numDiffSamplesToSync = 20，这个值就是这20个diff中最老的那个diff（也就是第一个）的权值，
   * 例如这里是0.01，那么随着diff的新旧程度，权值会逐渐增大，设分别为a1=0.01, a2, a3, ... a20=1
   * 最后的加权值是a1*diff1 + a2*diff2 + ... + a20*diff20/(a1+a2+...+a20)
   * @remark 这个值不能太大，因为后面为了计算简略，利用到了这个要求，要求<=0.05
   */
  const double weightOfOldestDiff;

public:
  constexpr PlayerInnerConfig(const double tooBigDiffNoSync, const uint16_t numDiffSamplesToSync, const double weightOfOldestDiff) :
    tooBigDiffNoSync(tooBigDiffNoSync),
    numDiffSamplesToSync(numDiffSamplesToSync),
    weightOfOldestDiff(weightOfOldestDiff) {
    assert(weightOfOldestDiff <= 0.05);
  }

  [[nodiscard]] constexpr double getTooBigDiffNoSync() const {
    return tooBigDiffNoSync;
  }

  [[nodiscard]] constexpr double getNumDiffSamplesToSync() const {
    return numDiffSamplesToSync;
  }
};

#endif //PLAYER_INNER_CONFIG_H
