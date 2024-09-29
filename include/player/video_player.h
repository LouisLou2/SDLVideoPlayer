//
// Created by leo on 24-9-29.
//

#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H
#include <string_view>
#include <struct/packet_queue.h>

#include "struct/frame_queue.h"


class VideoPlayer {
protected:
  std::string_view video_path;
  PacketQueue vPktq;
  PacketQueue aPktq;
  PacketQueue sPktq;

  FrameQueue vFrameq;
  FrameQueue aFrameq;
  FrameQueue sFrameq;

public:
  explicit VideoPlayer(std::string_view video_path);
  virtual void play() = 0;
  ~VideoPlayer() = default;
};



#endif //VIDEO_PLAYER_H
