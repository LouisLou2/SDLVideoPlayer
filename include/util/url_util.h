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
  static constexpr std::array<std::string_view, 17> netProtocols = {
    "http://",
    "https://",
    "ftp://",
    "rtsp://",
    "rtmp://",
    "udp://",
    "tcp://",
    "rtp://",
    "sdp://",
    "mms://",
    "hls://",
    "dash://",
    "webrtc://",
    "srt://",
    "ice://",
    "quic://",
    "mcast://"
  };
  static constexpr std::array<StreamProtocol, 17> netStProtocols = {
    StreamProtocol::Http,
    StreamProtocol::Https,
    StreamProtocol::Ftp,
    StreamProtocol::Rtsp,
    StreamProtocol::Rtmp,
    StreamProtocol::Udp,
    StreamProtocol::Tcp,
    StreamProtocol::Rtp,
    StreamProtocol::Sdp,
    StreamProtocol::Mms,
    StreamProtocol::Hls,
    StreamProtocol::Dash,
    StreamProtocol::Webrtc,
    StreamProtocol::Srt,
    StreamProtocol::Ice,
    StreamProtocol::Quic,
    StreamProtocol::Mcast
  };
public:
  static inline StreamProtocol getProtocolType(const std::string& url);
};

inline StreamProtocol UrlUtil::getProtocolType(const std::string& url) {
  // file:// 开头，则是本地文件, 如果不以字母开头，说明不是网络协议，也是本地文件
  if (!std::isalpha(url[0]) || url.starts_with("file://") ) {
    return StreamProtocol::File;
  }
  for (uint8_t i = 0; i < netProtocols.size(); ++i) {
    if (url.starts_with(netProtocols[i])) {
      return netStProtocols[i];
    }
  }
  return StreamProtocol::File;
}


#endif //URL_UTIL_H
