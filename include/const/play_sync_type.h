//
// Created by leo on 24-10-4.
//

#ifndef PLAY_SYNC_TYPE_H
#define PLAY_SYNC_TYPE_H
#include <cstdint>

enum class PlaySyncType: uint8_t {
  AudioMaster = 0,
  VideoMaster = 1,
  ExternalMaster = 2,
};

#endif //PLAY_SYNC_TYPE_H
