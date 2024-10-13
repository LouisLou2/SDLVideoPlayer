//
// Created by leo on 24-10-13.
//

#ifndef PLAY_ENVS_H
#define PLAY_ENVS_H
#include <cassert>
#include <vector>
#include <limits>
#include "entity/screen_info.h"

class PlayEnvs {
protected:
  static bool screenInfoSet;
  // 大小不可大于等于uint16_t
  static std::vector<ScreenInfo> screenInfos;
public:
  // static inline void setScreenInfos(std::vector<ScreenInfo>&& infos);
  static inline uint16_t getScreenNum() noexcept;
  static void ensureScreenInfoSet();

  static inline bool isScreenInfoSet() noexcept;
};

//
// inline void PlayEnvs::setScreenInfos(std::vector<ScreenInfo>&& infos) {
//   assert(!screenInfoSet);
//   assert(infos.size() < std::numeric_limits<uint16_t>::max());
//   screenInfos = std::move(infos);
//   screenInfoSet = true;
// }

inline uint16_t PlayEnvs::getScreenNum() noexcept{
  return screenInfos.size();
}

inline bool PlayEnvs::isScreenInfoSet() noexcept {
  return screenInfoSet;
}

#endif //PLAY_ENVS_H
