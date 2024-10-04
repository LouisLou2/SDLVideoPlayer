//
// Created by leo on 24-10-4.
//

#ifndef STREAM_PROTOCOL_H
#define STREAM_PROTOCOL_H
#include <cstdint>

enum class StreamProtocol : uint8_t{
  File = 0,
  Http = 1,
  Https = 2,
  Ftp = 3,
  Rtsp = 4,
  Rtmp = 5
};

class StreamProtocolUtil {
public:
  static inline bool isLocalProtocol(StreamProtocol protocol);
};

inline bool StreamProtocolUtil::isLocalProtocol(StreamProtocol protocol) {
  return protocol == StreamProtocol::File;
}

#endif //STREAM_PROTOCOL_H
