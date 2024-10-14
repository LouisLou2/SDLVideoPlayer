//
// Created by leo on 24-10-15.
//

#ifndef SDL_UTIL_H
#define SDL_UTIL_H
#include <array>
#include <optional>
#ifdef __cplusplus
extern "C" {
#include <SDL_audio.h>
#include <libavutil/samplefmt.h>
}
#else
#include <SDL_audio.h>
#include <libavutil/samplefmt.h>
#endif

class SDLUtil {
  // 定义AVSampleFormat和SDL_AudioFormat的对应关系
  static constexpr std::array<std::pair<AVSampleFormat, SDL_AudioFormat>, 4> formatPairList = {{
    { AV_SAMPLE_FMT_U8,   AUDIO_U8 },      // FFmpeg 无符号 8-bit 整数 -> SDL 无符号 8-bit 整数
    { AV_SAMPLE_FMT_S16,  AUDIO_S16SYS },  // FFmpeg 有符号 16-bit 整数 -> SDL 有符号 16-bit 整数 (系统字节序)
    { AV_SAMPLE_FMT_S32,  AUDIO_S32SYS },  // FFmpeg 有符号 32-bit 整数 -> SDL 有符号 32-bit 整数 (系统字节序)
    { AV_SAMPLE_FMT_FLT,  AUDIO_F32SYS }   // FFmpeg 32-bit 浮点数 -> SDL 32-bit 浮点数 (系统字节序)
  }};
public:
  static std::optional<AVSampleFormat> getCorAVFormat(SDL_AudioFormat sdlAudFmt);
};

#endif //SDL_UTIL_H
