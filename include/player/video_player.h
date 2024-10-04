//
// Created by leo on 24-9-29.
//

#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

class VideoPlayer {
public:
  explicit VideoPlayer() = default;
  virtual void play() = 0;
  virtual ~VideoPlayer() = default;
};



#endif //VIDEO_PLAYER_H
