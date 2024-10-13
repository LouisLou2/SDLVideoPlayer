//
// Created by leo on 24-10-10.
//
#include <utility>
#include "player/sdl_video_player/sdl_media_filter_info.h"
#include "player/sdl_video_player/sdl_video_player.h"
#include "util/ff_util.h"

#ifdef __cplusplus
extern "C" {
#include <libavutil/samplefmt.h>
#include <libavutil/bprint.h>
#include <libavutil/opt.h>
}
#else
#include <libavutil/samplefmt.h>
#include <libavutil/bprint.h>
#include <libavutil/opt.h>
#endif

#define FILTER_PAD_FOR_SINGLE 0

std::optional<ErrorDesc> SDLMediaFilterInfo::configAudioFilter(
  const std::map<std::string,std::string>& swrOpts,
  const std::string& audioFilterGraphStr,
  uint8_t filterThreadsNum,
  bool forceOutputFormat
) {
  std::optional<ErrorDesc> optErr = std::nullopt;
  int ret = 0;
  AVBPrint bp;// 为了配合ffmpeg的接口
  std::array<AVSampleFormat,2> sampleFmtPair = {
    THE_ONLY_ALLOWED_SAMPLE_FMT_FF, AV_SAMPLE_FMT_NONE
  };
  std::array<int,2> sampleRatePair = {0, -1};
  AVFilterContext* filterAudioSrc = nullptr;
  AVFilterContext* filterAudioSink = nullptr;
  std::string filterArgsStr;
  std::string swrOptsPairStr;
  // 释放graph, 为什么？ 因为这个函数可能会被多次调用，要把前一次的graph释放掉
  avfilter_graph_free(&audFilterGraph);
  // 重新创建graph
  audFilterGraph = avfilter_graph_alloc(); // 没存分配应该不会出错吧？
  if (!audFilterGraph) {
    optErr = ErrorDesc::from(ExceptionType::MemoryAllocFailed, "avfilter_graph_alloc failed");
    goto end;
  }
  // 设置滤镜线程数
  audFilterGraph->nb_threads = filterThreadsNum;
  swrOptsPairStr = FFUtil::getAllPairString(swrOpts, ":");
  audFilterGraph->aresample_swr_opts = av_strdup(swrOptsPairStr.data()); // configureAuduoFilter函数退出后，swrOptsPairStr的内存释放掉， 但是那是好像这个字段也已经作用发挥完了，之后再继续观察学习， 看是不是有问题

  // 在使用bp之前，要初始化
  av_bprint_init(&bp, 0, AV_BPRINT_SIZE_AUTOMATIC);
  av_channel_layout_describe_bprint(&audFilterParamsSrc.ch_layout, &bp);
  // 准备创建滤镜的参数字符串
  filterArgsStr = std::format(
    "sample_rate={}:sample_fmt={}:time_base={}/{}:channel_layout={}",
    audFilterParamsSrc.freq,
    av_get_sample_fmt_name(audFilterParamsSrc.fmt),
    1,
    audFilterParamsSrc.freq,
    bp.str
  );
  // 创建滤镜src
  ret = avfilter_graph_create_filter(
    &filterAudioSrc,
    avfilter_get_by_name("abuffer"),
    "player_abuffer",
    filterArgsStr.data(),
    nullptr,
    audFilterGraph
  );
  // 检查创建滤镜是否成功
  if (ret < 0) {
    optErr = ErrorDesc::from(
      ExceptionType::FFComponentInitFailed,
      std::string("create abuffer filter failed: ") + av_err2str(ret)
    );
    goto end;
  }
  // 创建滤镜sink
  ret = avfilter_graph_create_filter(
    &filterAudioSink,
    avfilter_get_by_name("abuffersink"),
    "player_abuffersink",
    nullptr,
    nullptr,
    audFilterGraph
  );
  // 检查创建滤镜是否成功
  if (ret < 0) {
    optErr = ErrorDesc::from(
      ExceptionType::FFComponentInitFailed,
      std::string("create abuffersink filter failed: ") + std::string(av_err2str(ret))
    );
    goto end;
  }
  // 设置滤镜sink的参数: 这里是仅仅设置了sample_fmts，注意标志是AV_OPT_SEARCH_CHILDREN， 代表子成员也在搜索范围内
  if((ret = av_opt_set_int_list(filterAudioSink, "sample_fmts", sampleFmtPair.data(), sampleFmtPair.back(), AV_OPT_SEARCH_CHILDREN) < 0)){
    optErr = ErrorDesc::from(
      ExceptionType::FFmpegSetFailed,
      std::string("set sample_fmts failed: ") + std::string(av_err2str(ret))
    );
    goto end;
  }
  if ((ret = av_opt_set_int(filterAudioSink, "all_channel_counts", 1, AV_OPT_SEARCH_CHILDREN)) < 0) {
    optErr = ErrorDesc::from(
      ExceptionType::FFmpegSetFailed,
      std::string("set all_channel_counts failed: ") + std::string(av_err2str(ret))
    );
    goto end;
  }
  // 如果强制设置输出格式
  if (forceOutputFormat) {
    sampleRatePair.front() = audParamsTarget.freq;
    // 设置不能接受音频所有通道，而需要使用audioParamsTarget的通道
    if ((ret = av_opt_set_int(filterAudioSink, "all_channel_counts", 0, AV_OPT_SEARCH_CHILDREN)) < 0) {
      optErr = ErrorDesc::from(
        ExceptionType::FFmpegSetFailed,
        std::string("set all_channel_counts failed: ") + std::string(av_err2str(ret))
      );
      goto end;
    }
    // TODO: Confused Why use bp.str, 第一次调用不会进入这个分支，到第二次进入时在回来看看
    if ((ret = av_opt_set(filterAudioSink,"ch_layouts", bp.str, AV_OPT_SEARCH_CHILDREN)) < 0) {
      optErr = ErrorDesc::from(
        ExceptionType::FFmpegSetFailed,
        std::string("set ch_layouts failed: ") + std::string(av_err2str(ret))
      );
      goto end;
    }
    if ((ret = av_opt_set_int_list(filterAudioSink, "sample_rates", sampleRatePair.data(), sampleRatePair.back(), AV_OPT_SEARCH_CHILDREN)) < 0) {
      optErr = ErrorDesc::from(
        ExceptionType::FFmpegSetFailed,
        std::string("set sample_rates failed: ") + std::string(av_err2str(ret))
      );
      goto end;
    }
  }
  // 开始配置滤镜图: 经过这一步（如果没出错的话），滤镜图一连串已经连接好了
  optErr = configAudioFilterGraph(audioFilterGraphStr, filterAudioSrc, filterAudioSink);
  if (optErr.has_value()) {
    goto end;
  }
  // 把开始和结束的滤镜都保存起来
  audioFilterIn = filterAudioSrc;
  audFilterOut = filterAudioSink;
end:
  av_bprint_finalize(&bp, nullptr);
  // 错误时才释放滤镜图
  if (ret < 0) {
    avfilter_graph_free(&audFilterGraph);
  }
  return optErr;
}

std::optional<ErrorDesc> SDLMediaFilterInfo::configAudioFilterGraph(
  const std::string& audioFilterGraphStr,
  AVFilterContext* filterIn,
  AVFilterContext* filterOut) {
  int ret;
  std::optional<ErrorDesc> optErr = std::nullopt;
  AVFilterInOut* input = nullptr, *output = nullptr;
  uint32_t oldNbFilters = audFilterGraph->nb_filters;

  if (audioFilterGraphStr.empty()) {
    // 不使用字符串配置滤镜图，直接将输入输出连接起来
    // 这一步会将filterIn的输出字段连接到filterOut的输入字段，也就是两个对象都进行了修改，但是这都跟graph无关
    if ((ret = avfilter_link(filterIn, FILTER_PAD_FOR_SINGLE, filterOut, FILTER_PAD_FOR_SINGLE)) < 0) {
      optErr = ErrorDesc::from(
        ExceptionType::FFmpegSetFailed,
        std::string("link filter failed: ") + std::string(av_err2str(ret))
      );
      goto end;
    }
  }else {
    // TODO: 这一步的滤镜配置很是迷惑，之后需要几个api的实现源码再看看
    input = avfilter_inout_alloc();
    output = avfilter_inout_alloc();
    if (!input || !output) {
      // alloc failed
      optErr = ErrorDesc::from(
        ExceptionType::MemoryAllocFailed,
        "avfilter_inout_alloc failed"
      );
      goto end;
    }
    input->name = av_strdup("out");
    input->filter_ctx = filterOut;
    input->pad_idx = FILTER_PAD_FOR_SINGLE;
    input->next = nullptr;

    output->name = av_strdup("in");
    output->filter_ctx = filterIn;
    output->pad_idx = FILTER_PAD_FOR_SINGLE;
    output->next = nullptr;
    /*
    * 数据流:
      [ source_ctx ] ---> (滤镜链) ---> [ sink_ctx ]

      滤镜链视角:
      [ source_ctx ] ---> outputs -> (filter graph) -> inputs ---> [ sink_ctx ]
     */
    if ((ret = avfilter_graph_parse_ptr(audFilterGraph, audioFilterGraphStr.data(), &input, &output, nullptr)) < 0) {
      optErr= ErrorDesc::from(
        ExceptionType::FFmpegSetFailed,
        std::string("parse filter graph failed: ") + std::string(av_err2str(ret))
      );
      goto end;
    }
  }
  for (uint32_t i = 0; i < audFilterGraph->nb_filters - oldNbFilters;++i) {
    FFSWAP(AVFilterContext*, audFilterGraph->filters[i], audFilterGraph->filters[i + oldNbFilters]);
  }
  // check and link all filters with order
  if ((ret = avfilter_graph_config(audFilterGraph, nullptr))< 0) {
    optErr = ErrorDesc::from(
      ExceptionType::FFmpegSetFailed,
      std::string("config filter graph failed: ") + std::string(av_err2str(ret))
    );
    goto end;
  }
end:
  avfilter_inout_free(&input);
  avfilter_inout_free(&output);
  return optErr;
}
