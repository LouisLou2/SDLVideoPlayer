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

#define BIG_OPT_SIZE 128

bool FFUtil::isRealTime(const AVFormatContext* fmtCtx) {
  // 检查input format是否是realtime
  for (const char* rtFmt : realTimeFormats)
    if (strcmp(fmtCtx->iformat->name, rtFmt) == 0) return true;
  // 检查url的前四个字符
  if (strncmp(fmtCtx->url, "rtp:", 4) == 0 || strncmp(fmtCtx->url, "udp:", 4) == 0)
    return true;
  return false;
}

std::vector<AVDictionaryEntry*> FFUtil::getAllEntries(const AVDictionary* dict) {
  if (!dict) return {};
  std::vector<AVDictionaryEntry*> res;
  AVDictionaryEntry* entry = nullptr;
  while ((entry = av_dict_get(dict, "", entry, AV_DICT_IGNORE_SUFFIX))) {
    res.push_back(entry);
  }
  return res;
}

std::string FFUtil::stringifyAVDictionary(const std::vector<AVDictionaryEntry*>& entries) {
  std::string res("{");
  for (const AVDictionaryEntry* entry : entries) {
    // 拼接少量字符串，不用std::format和stringstream了
    res += std::string(entry->key) + ":" + std::string(entry->value) + ", ";
  }
  // 删除最后一个空格
  if (res.size() > 1) res.pop_back();
  // 还剩最后一个逗号，改为右括号
  res.back() = '}';
  return res;
}

std::string FFUtil::getAllPairString(const AVDictionary* dict, const char* split) {
  std::string res;
  res.reserve(BIG_OPT_SIZE); // 默认大小， 超出就警报
  const AVDictionaryEntry* e = nullptr;
  bool warn = false; // 只警告一次
  while ((e = av_dict_iterate(dict,e))) {
    res += e->key;
    res += '=';
    res += e->value;
    res += split;
    if (res.size() > BIG_OPT_SIZE && !warn) {
      warn = true;
      PlayerLogger::log(
        ErrorDesc::from(
          ExceptionType::BigConfiguration,
          "the size of the configuration is a little big exceed " + std::to_string(BIG_OPT_SIZE) + " bytes"
        )
      );
    }
  }
  // 删除最后一个split
  if (!res.empty()) res.pop_back();
  return res;
}

std::string FFUtil::getAllPairString(const std::map<std::string, std::string>& dict, const char* split) {
  std::string res;
  res.reserve(BIG_OPT_SIZE); // 默认大小， 超出就警报
  bool warn = false; // 只警告一次
  for (const auto& [key, value] : dict) {
    res += key;
    res += '=';
    res += value;
    res += split;
    if (res.size() > BIG_OPT_SIZE && !warn) {
      warn = true;
      PlayerLogger::log(
        ErrorDesc::from(
          ExceptionType::BigConfiguration,
          "the size of the configuration is a little big exceed " + std::to_string(BIG_OPT_SIZE) + " bytes"
        )
      );
    }
  }
  // 删除最后一个split
  if (!res.empty()) res.pop_back();
  return res;
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
