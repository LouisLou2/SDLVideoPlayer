//
// Created by leo on 24-10-4.
//

#ifndef VIDEO_BASIC_INFO_H
#define VIDEO_BASIC_INFO_H
#include <string>

#include "const/stream_protocol.h"

#ifdef __cplusplus
extern "C" {
#include <libavformat/avformat.h>
}
#else
#include <libavformat/avformat.h>
#endif

struct VideoBasicInfo {
  friend class SDLVideoPlayer;
private:
  StreamProtocol protocol;
  std::string filename;
  const AVInputFormat *iformat; // 输入格式
};



#endif //VIDEO_BASIC_INFO_H
