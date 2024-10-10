//
// Created by leo on 24-10-10.
//

#ifndef SDL_CACHE_COLLECTION_H
#define SDL_CACHE_COLLECTION_H
#include <struct/frame_queue.h>
#include <struct/packet_queue.h>

class SDLPlayerCacheCollection {
  friend class SDLVideoPlayer;
  PacketQueue vPktq;
  PacketQueue aPktq;
  PacketQueue sPktq;
  FrameQueue vFrameq;
  FrameQueue aFrameq;
  FrameQueue sFrameq;
public:
  SDLPlayerCacheCollection(
    size_t vFrameqSize, bool vFrameqKeepLast,
    size_t aFrameqSize, bool aFrameqKeepLast,
    size_t sFrameqSize, bool sFrameqKeepLast
    );
};

inline SDLPlayerCacheCollection::SDLPlayerCacheCollection(
  size_t vFrameqSize, bool vFrameqKeepLast,
  size_t aFrameqSize, bool aFrameqKeepLast,
  size_t sFrameqSize, bool sFrameqKeepLast
  ):
  vFrameq(vFrameqSize, vFrameqKeepLast),
  aFrameq(aFrameqSize, aFrameqKeepLast),
  sFrameq(sFrameqSize, sFrameqKeepLast) {}

#endif //SDL_CACHE_COLLECTION_H
