//
// Created by leo on 24-9-30.
//
#ifndef FF_MEM_H
#define FF_MEM_H

#ifdef __cplusplus
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/bsf.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
#else
include <libavformat/avformat.h>
#include <libavcodec/bsf.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#endif

struct AVDictionaryDeleter {
  void operator()(AVDictionary* dict) const {
    av_dict_free(&dict);
  }
};

// deleter for AVFormatContext
struct AVFormatContextDeleter {
  void operator()(AVFormatContext* fmtCtx) const {
    avformat_close_input(&fmtCtx);
  }
};

// deleter for AVPacket
struct AVPacketDeleter {
  void operator()(AVPacket* pkt) const {
    av_packet_free(&pkt);
  }
};

// deleter for AVBSFContext
struct AVBSFContextDeleter {
  void operator()(AVBSFContext* bsfCtx) const {
    av_bsf_free(&bsfCtx);
  }
};

struct AVCodecContextDeleter {
  void operator()(AVCodecContext* codecCtx) const {
    avcodec_free_context(&codecCtx);
  }
};

struct AVFrameDeleter {
  void operator()(AVFrame* frame) const {
    av_frame_free(&frame);
  }
};

struct AVParserContextDeleter {
  void operator()(AVCodecParserContext* parserCtx) const {
    av_parser_close(parserCtx);
  }
};

struct AVCodecDeleter {
  void operator()(const AVCodec* codec) const {
    // static no need to free
  }
};

struct SwsContextDeleter {
  void operator()(SwsContext* swsCtx) const {
    sws_freeContext(swsCtx);
  }
};

struct SwrContextDeleter {
  void operator()(SwrContext* swrCtx) const {
    swr_free(&swrCtx);
  }
};
#endif //FF_MEM_H
