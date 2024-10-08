//
// Created by leo on 24-10-6.
//
#include "util/ff_util.h"

#include "player/sdl_video_player/sdl_vid_player_setting.h"

bool FFUtil::isRealTime(const AVFormatContext* fmtCtx) {
  // 检查input format是否是realtime
  for (const char* rtFmt : realTimeFormats)
    if (strcmp(fmtCtx->iformat->name, rtFmt) == 0) return true;
  // 检查url的前四个字符
  if (strncmp(fmtCtx->url, "rtp:", 4) == 0 || strncmp(fmtCtx->url, "udp:", 4) == 0)
    return true;
  return false;
}
