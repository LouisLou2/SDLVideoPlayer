//
// Created by leo on 24-10-4.
//

#ifndef URL_UTIL_H
#define URL_UTIL_H

#include <array>
#include <string>

#include "const/stream_protocol.h"

class UrlUtil {
  // 以下两个数组的顺序必须一一对应
  static constexpr std::array<std::string_view, 5> netProtocols = {
    "http://",
    "https://",
    "ftp://",
    "rtsp://",
    "rtmp://"
  };
  static constexpr std::array<StreamProtocol, 5> netStProtocols = {
    StreamProtocol::Http,
    StreamProtocol::Https,
    StreamProtocol::Ftp,
    StreamProtocol::Rtsp,
    StreamProtocol::Rtmp
  };
public:
  static inline StreamProtocol getProtocolType(const std::string& url);
};

inline StreamProtocol UrlUtil::getProtocolType(const std::string& url) {
  for (uint8_t i = 0; i < netProtocols.size(); ++i) {
    if (url.starts_with(netProtocols[i])) {
      return netStProtocols[i];
    }
  }
  return StreamProtocol::File;
}


#endif //URL_UTIL_H
