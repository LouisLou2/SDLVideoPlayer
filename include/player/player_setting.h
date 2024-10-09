//
// Created by leo on 24-10-8.
//

#ifndef PLAYER_SETTING_H
#define PLAYER_SETTING_H
#include <vector>
#include "entity/screen_info.h"

class PlayerSetting {
protected:
  static bool screenInfoSet;
  static std::vector<ScreenInfo> screenInfo;
  static inline void setScreenInfos(std::vector<ScreenInfo>&& infos);
public:
  virtual ~PlayerSetting() = default;
};

inline void PlayerSetting::setScreenInfos(std::vector<ScreenInfo>&& infos) {
  screenInfo = std::move(infos);
  screenInfoSet = true;
}


#endif //PLAYER_SETTING_H
