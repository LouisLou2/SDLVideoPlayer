//
// Created by leo on 24-10-4.
//

#ifndef STREAM_PROTOCOL_H
#define STREAM_PROTOCOL_H
#include <cstdint>

enum class StreamProtocol : uint8_t {
  File = 0,    // 本地文件
  Http = 1,    // HTTP 协议
  Https = 2,   // HTTPS 协议
  Ftp = 3,     // FTP 协议
  Rtsp = 4,    // RTSP 实时流协议
  Rtmp = 5,    // RTMP 实时消息协议
  Udp = 6,     // UDP 用户数据报协议
  Tcp = 7,     // TCP 传输控制协议
  Rtp = 8,     // RTP 实时传输协议
  Sdp = 9,     // SDP 会话描述协议
  Mms = 10,    // MMS 微软媒体服务器协议
  Hls = 11,    // HLS HTTP 实时流协议
  Dash = 12,   // DASH 自适应流协议
  Webrtc = 13, // WebRTC 实时通信协议
  Srt = 14,    // SRT 安全可靠传输协议
  Ice = 15,    // ICE 交互式连接建立协议
  Quic = 16,   // QUIC 快速 UDP 互联网连接协议
  Mcast = 17   // 多播协议
};

class StreamProtocolUtil {
public:
  static inline bool isLocalProtocol(StreamProtocol protocol);
};

inline bool StreamProtocolUtil::isLocalProtocol(StreamProtocol protocol) {
  return protocol == StreamProtocol::File;
}

#endif //STREAM_PROTOCOL_H
