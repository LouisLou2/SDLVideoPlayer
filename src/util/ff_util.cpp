//
// Created by leo on 24-10-6.
//
#include "util/ff_util.h"

bool FFUtil::isRealTime(const AVFormatContext* fmtCtx) {
  // 检查input format是否是realtime
  for (const char* rtFmt : realTimeFormats)
    if (strcmp(fmtCtx->iformat->name, rtFmt) == 0) return true;
  // 检查url的前四个字符
  if (strncmp(fmtCtx->url, "rtp:", 4) == 0 || strncmp(fmtCtx->url, "udp:", 4) == 0)
    return true;
  return false;
}

AVDictionary* FFUtil::parseFormatOpt(const PlayerSettings& settings) {
  // TODO: 并未实现
  return nullptr;
}

std::vector<AVDictionary*> FFUtil::
parseCodecOpt(const AVFormatContext* fmtCtx, const PlayerSettings& settings) {
  uint16_t stNum = fmtCtx->nb_streams;
  // TODO：具体就是根据settings的设置,来认领每个stream的AVDictionary,这里就之后再写那些复杂的option, 先把所有的dict初始化为nullptr, 直接传出去吧
  return std::vector<AVDictionary*> (stNum,nullptr);
}
