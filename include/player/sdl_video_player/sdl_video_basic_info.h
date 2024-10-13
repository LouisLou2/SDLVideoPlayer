//
// Created by leo on 24-10-4.
//

#ifndef VIDEO_BASIC_INFO_H
#define VIDEO_BASIC_INFO_H
#include <string>
#include <memory>
#include <array>
#include <optional>

#include "const/stream_protocol.h"
#include "util/mem/ff_mem.h"
#include "entity/error_desc.h"

#define MIN_STREAM_SIZE 1024

struct VideoBasicInfo {
  friend class SDLVideoPlayer;
private:
  StreamProtocol protocol;
  std::string originalUrl; // 注意这是用户提供的原始url，他可以是网络地址，也可以是本地地址（相对路径，绝对路径均可）
  std::unique_ptr<AVFormatContext, AVFormatContextDeleter> fmtCtx;
  const AVInputFormat *iformat; // 输入格式
  bool isRealTime;
  std::array<int32_t, AVMEDIA_TYPE_NB> streamIndex;
  // 很多字段不需要初始化，之后会被赋值的, 这个VideoBasicInfo基本就是只服务于SDLVideoPlayer的，所以设置为private
  VideoBasicInfo();
  // 对于刚刚传入的url进行分析，初步检查文件的合法性
  std::optional<ErrorDesc> checkFileAndSetBasic();
};
#endif //VIDEO_BASIC_INFO_H
