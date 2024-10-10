//
// Created by leo on 24-10-10.
//

#ifndef SDL_MEDIA_FILTER_INFO_H
#define SDL_MEDIA_FILTER_INFO_H

#include <map>
#include <optional>
#include "entity/error_desc.h"
#include "entity/audio_params.h"
#ifdef __cplusplus
extern "C" {
#include <libavfilter/avfilter.h>
}
#else
#include <libavfilter/avfilter.h>
#endif

class SDLMediaFilterInfo {
  friend class SDLVideoPlayer;
  AudioParams audioParamsSrc;
  AudioParams audioParamsDst;
  AudioParams audioFilterParamsSrc;
  AVFilterGraph* audioFilterGraph;

  AVFilterContext* audioFilterIn;
  AVFilterContext* audioFilterOut;
  AVFilterContext* videoFilterIn;
  AVFilterContext* videoFilterOut;

  // 构造函数私有，仅供友元类使用
  SDLMediaFilterInfo();
  // 提供私有函数，只给友元类使用
  std::optional<ErrorDesc> configAudioFilter(
    const std::map<std::string,std::string>& swrOpts,
    const std::map<std::string,std::string>& audioFilterOpts,
    uint8_t filterThreadsNum,
    bool forceOutputFormat = false
  );
};

inline SDLMediaFilterInfo::SDLMediaFilterInfo():
  audioFilterGraph(nullptr),
  audioFilterIn(nullptr),
  audioFilterOut(nullptr),
  videoFilterIn(nullptr),
  videoFilterOut(nullptr){}

#endif //SDL_MEDIA_FILTER_INFO_H
