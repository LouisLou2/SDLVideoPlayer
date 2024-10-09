//
// Created by leo on 24-10-6.
//
#include "util/ff_util.h"
#include "player/sdl_video_player/sdl_vid_player_setting.h"

#ifdef __cplusplus
#include <libavutil/rational.h>
#else
#include <libavutil/rational.h>
#endif

bool FFUtil::isRealTime(const AVFormatContext* fmtCtx) {
  // 检查input format是否是realtime
  for (const char* rtFmt : realTimeFormats)
    if (strcmp(fmtCtx->iformat->name, rtFmt) == 0) return true;
  // 检查url的前四个字符
  if (strncmp(fmtCtx->url, "rtp:", 4) == 0 || strncmp(fmtCtx->url, "udp:", 4) == 0)
    return true;
  return false;
}


std::tuple<uint32_t,uint32_t> FFUtil::getPictureDisplayRect(uint32_t fw, uint32_t fh, AVRational par, uint32_t wantedWidth, uint32_t wantedHeight) {
  uint32_t resW,resH;
  if (av_cmp_q(par,av_make_q(0,1))<=0) {
    par = av_make_q(1,1);// 如果par是0(这是无效的),则认为是1:1
  }
  AVRational dar = av_mul_q(av_make_q(fw, fh), par);
  resW = av_rescale(wantedHeight, dar.num, dar.den) & ~1;// 保证是偶数, 因为某些解码器要求是偶数
  if (resW > wantedWidth) {
    resW = wantedWidth;
    resH = av_rescale(wantedWidth, dar.den, dar.num) & ~1;
  }
  resW = std::max(resW, 1u);
  resH = std::max(resH, 1u);
  return std::make_tuple(resW,resH);
}
