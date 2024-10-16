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
  PacketQueue vPktq;
  PacketQueue aPktq;
  PacketQueue sPktq;

};

#endif //SDL_CACHE_COLLECTION_H
