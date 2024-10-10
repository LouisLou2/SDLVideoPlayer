//
// Created by leo on 24-10-10.
//
#include <utility>
#include "player/sdl_video_player/sdl_media_filter_info.h"

#include <util/ff_util.h>
#ifdef __cplusplus
extern "C" {
#include <libavutil/samplefmt.h>
}
#else
#include <libavutil/samplefmt.h>
#endif
std::optional<ErrorDesc> SDLMediaFilterInfo::configAudioFilter(
  const std::map<std::string,std::string>& swrOpts,
  const std::map<std::string,std::string>& audioFilterOpts,
  uint8_t filterThreadsNum,
  bool forceOutputFormat
) {
  int ret;
  std::pair<AVSampleFormat, AVSampleFormat> sampleFmtPair = {
    AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE
  };
  std::pair<int, int> sampleRates = {0, -1};
  AVFilterContext* filterAudioSrc = nullptr;
  AVFilterContext* filterAudioSink = nullptr;
  // 512个字节
  char resampleSwsOpts[512] = "";
  // 释放graph, 为什么？ 因为这个函数可能会被多次调用，要把前一次的graph释放掉
  avfilter_graph_free(&audioFilterGraph);
  // 重新创建graph
  audioFilterGraph = avfilter_graph_alloc(); // 没存分配应该不会出错吧？
  if (!audioFilterGraph) {
    return ErrorDesc::from(ExceptionType::MemoryAllocFailed, "avfilter_graph_alloc failed");
  }
  // 设置滤镜线程数
  audioFilterGraph->nb_threads = filterThreadsNum;
  auto swrOptsPairStr = FFUtil::getAllPairString(swrOpts, ":");
  audioFilterGraph->aresample_swr_opts = swrOptsPairStr.data(); // 函数退出后，swrOptsPairStr的内存释放掉， 但是那是好像这个字段也已经作用发挥完了，之后再继续观察学习， 看是不是有问题

  // TODO: UNFINISHED
  return std::nullopt;
}
