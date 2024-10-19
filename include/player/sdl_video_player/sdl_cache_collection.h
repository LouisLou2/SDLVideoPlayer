//
// Created by leo on 24-10-10.
//

#ifndef SDL_CACHE_COLLECTION_H
#define SDL_CACHE_COLLECTION_H
#include <struct/frame_queue.h>
#include <struct/packet_queue.h>

class SDLPlayerCacheCollection {
  friend class SDLVideoPlayer;

  // packet queue are autogrowing
  PacketQueue aPktq;
  PacketQueue vPktq;
  PacketQueue sPktq;

  FrameQueue aFrameq;
  FrameQueue vFrameq;
  FrameQueue sFrameq;

public:
  SDLPlayerCacheCollection(
    size_t aFrameqSize,
    size_t vFrameqSize,
    size_t sFrameqSize,
    bool aFrameqKeepLast,
    bool vFrameqKeepLast,
    bool sFrameqKeepLast
  );

  FrameQueue& getAudFrameqRef() { return aFrameq; }
  FrameQueue& getVidFrameqRef() { return vFrameq; }
  FrameQueue& getSubFrameqRef() { return sFrameq; }
  const uint32_t& getAudPktqSerialRef() const { return aPktq.getSerialRef();}
  const uint32_t& getVidPktqSerialRef() const { return vPktq.getSerialRef();}
  const uint32_t& getSubPktqSerialRef() const { return sPktq.getSerialRef();}
};

inline SDLPlayerCacheCollection::SDLPlayerCacheCollection(
  size_t aFrameqSize,
  size_t vFrameqSize,
  size_t sFrameqSize,
  bool aFrameqKeepLast,
  bool vFrameqKeepLast,
  bool sFrameqKeepLast):
aFrameq(aFrameqSize, aFrameqKeepLast),
vFrameq(vFrameqSize, vFrameqKeepLast),
sFrameq(sFrameqSize, sFrameqKeepLast) {}
#endif //SDL_CACHE_COLLECTION_H
