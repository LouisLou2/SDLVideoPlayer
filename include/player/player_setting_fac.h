//
// Created by leo on 24-10-8.
//

#ifndef PLAYER_SETTING_FAC_H
#define PLAYER_SETTING_FAC_H
#include <utility>
#include "entity/error_desc.h"
#include "player_setting.h"
#include "player/sdl_video_player/sdl_vid_player_setting.h"

enum class PlayerTypeEnum {
  SDLPlayer,
  OtherPlayer,
};

class PlayerSettingFac {
public:
  template <typename ...Args>
  static PlayerSetting* createPlayerSetting(PlayerTypeEnum type, Args&&...args);
};

template <typename ...Args>
PlayerSetting* PlayerSettingFac::createPlayerSetting(PlayerTypeEnum type, Args&&...args) {
  switch (type) {
  case PlayerTypeEnum::SDLPlayer:
    return new SDLVidPlayerSettings(std::forward<Args>(args)...);
  default:
    throw ErrorDesc::from(ExceptionType::Unimplemented, "Player type not implemented");
  }
}

#endif //PLAYER_SETTING_FAC_H
