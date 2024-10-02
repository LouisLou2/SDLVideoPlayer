//
// Created by leo on 24-9-29.
//

#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H
#include "video_player.h"

class SDLVideoPlayer final : public VideoPlayer{
private:
  PacketQueue vPktq;
  PacketQueue aPktq;
  PacketQueue sPktq;

  FrameQueue vFrameq;
  FrameQueue aFrameq;
  FrameQueue sFrameq;
public:
  explicit SDLVideoPlayer(std::string_view video_path);
  void play() override;
};



#endif //VIDEOPLAYER_H
