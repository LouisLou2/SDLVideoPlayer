//
// Created by leo on 24-10-4.
//
#include "player/sdl_video_player/sdl_vid_player_setting.h"

AVDictionary* SDLVidPlayerSettings::getFormatOpt() const{
  // TODO: unimplmented
  return nullptr;
}

std::vector<AVDictionary*> SDLVidPlayerSettings::getCodecOpts(const AVFormatContext* fmtCtx) {
  uint16_t stNum = fmtCtx->nb_streams;
  // TODO：具体就是根据settings的设置,来认领每个stream的AVDictionary,这里就之后再写那些复杂的option, 先把所有的dict初始化为nullptr, 直接传出去吧
  return std::vector<AVDictionary*> (stNum,nullptr);
}