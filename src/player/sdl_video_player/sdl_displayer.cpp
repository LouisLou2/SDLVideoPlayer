//
// Created by leo on 24-10-13.
//
#include "player/sdl_video_player/sdl_displayer.h"

#include <util/ff_util.h>

#include "player/sdl_video_player/sdl_video_player.h"
#include "util/sdl_util.h"
#ifdef __cplusplus
extern "C"{
#include <SDL2/SDL_audio.h>
}
#else
#include <SDL2/SDL_audio.h>
#endif
#define SILENCE 0

AudioParams SDLDisplayer::copySDLAudioSpecToAudioParams(const SDL_AudioSpec& spec, const AVChannelLayout* ch_layout) noexcept(false) {
  AudioParams paras;
  auto ffFmt = SDLUtil::getCorAVFormat(spec.format);
  if (!ffFmt) {
    throw ErrorDesc::from(ExceptionType::FFmpegSetFailed, "Invalid audio format");
  }
  paras.setFmt(ffFmt.value());
  paras.setFreq(spec.freq);
  paras.copySetChLayout(ch_layout);
  uint32_t frameSize = av_samples_get_buffer_size(nullptr, spec.channels, 1, ffFmt.value(), 1);
  paras.setSizes(frameSize, spec.freq);
  return paras;
}

std::optional<ErrorDesc> SDLDisplayer::initDisplayer(const SDLVidPlayerSettings& settings) {
  uint32_t flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
  if (settings.disableAud) flags &= ~SDL_INIT_AUDIO;
  if (settings.disableVid) flags &= ~SDL_INIT_VIDEO;
  if (SDL_Init(flags)) {
    // 初始化失败
    return ErrorDesc::from(ExceptionType::SDLInitFailed, SDL_GetError());
  }
  // 使SDL不处理这些事件，以便我们自己处理
  SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
  SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

  if (settings.disableVid) return std::nullopt;

  uint32_t sdlFlags = SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;
  if (settings.alwaysOnTop) sdlFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
  if (settings.borderless) sdlFlags |= SDL_WINDOW_BORDERLESS;
  // 禁用窗口合成器
  SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
  // 设置渲染器缩放质量
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  // 创建窗口
  window.reset(SDL_CreateWindow(
    settings.windowTitle.c_str(),
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    settings.windowWidth,
    settings.windowHeight,
    sdlFlags
  ));
  if (window) {
    // 创建渲染器
    renderer.reset(
    SDL_CreateRenderer(window.get(),
      -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
      )
    );
    if (!renderer) {
      PlayerLogger::log(ErrorDesc::from(ExceptionType::SDLHardwareAccelerationFailed, SDL_GetError()));
      // 如果创建失败，尝试不使用硬件加速
      renderer.reset(
        SDL_CreateRenderer(
        window.get(),
        -1,
        0)
      );
    }
  }
  if (renderer) {
    // 获取渲染器信息
    if (!SDL_GetRendererInfo(renderer.get(), &rendererInfo))
      // 说明renderInfo获取没有问题
      PlayerLogger::log(LogLevel::Info, std::string("Renderer Initialized: ") + rendererInfo.name);
  }
  // 检查一系列创建是否成功
  if (!window || !renderer || !rendererInfo.num_texture_formats) {
    return ErrorDesc::from(ExceptionType::SDLComponentInitFailed, SDL_GetError());
  }
  return std::nullopt;
}

AudioParams SDLDisplayer::configAudioDisplay(
  SDL_AudioCallback callback,
  AVChannelLayout* wantedChLayOut,
  uint32_t wantedSR,
  SDL_AudioFormat mustBeFormat
  )  noexcept(false)
{
  static_assert(srAlternatives.size() < std::numeric_limits<uint8_t>::max() && !srAlternatives.empty());
  static_assert(chNumAlternatives.size() < std::numeric_limits<uint8_t>::max() && !chNumAlternatives.empty());
  assert(wantedChLayOut);
  assert(wantedSR > 0);

  SDL_AudioSpec wantedSpec, obtainedSpec;
  bool needRelayout = false;
  uint8_t chosenChNum = wantedChLayOut->nb_channels;
  if (const char* chNumStr = SDL_getenv("SDL_AUDIO_CHANNELS")) {
    // 如果环境变量中有设置，就用环境变量中的值
    chosenChNum = std::stoi(chNumStr);
    assert(chosenChNum > 0);
    needRelayout = true;
  }
  if (wantedChLayOut->order != AV_CHANNEL_ORDER_NATIVE) needRelayout = true;
  if (needRelayout) FFUtil::relayoutChannel(wantedChLayOut, chosenChNum);
  // chosenChNum接下来不会再改变了，其意义就是在不打开设备的情况下，最终确定的声道数
  // 检查设置好的声道数是否合法
  if (wantedChLayOut->nb_channels <= 0) {
    // 抛出异常没关系了，一是这种异常是不可恢复的，二是没有需要释放的资源
    throw ErrorDesc::from(ExceptionType::FFmpegSetFailed, "Invalid channel number");
  }
  // 设置wantedSpec
  wantedSpec.channels = wantedChLayOut->nb_channels;//调整后的声道数
  wantedSpec.freq = wantedSR; // 采样率先这样设置，也不做检查是否不符合通用的采样率标准，如果能打开最好，打不开再调整
  wantedSpec.format = mustBeFormat;
  wantedSpec.silence = SILENCE;
  wantedSpec.samples = FFUtil::getSamplesPerSec(wantedSpec.freq,wantedCallsPerSec,maxCallsPerSec,minSamplesContainedIn1Call);
  wantedSpec.callback = callback;
  audioDeviceId = SDL_OpenAudioDevice(
    nullptr,
    0,
    &wantedSpec,
    &obtainedSpec,
  SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE
  );
  auto srIter = srAlternatives.end();
  auto chIter = chNumAlternatives.end();
  if (!audioDeviceId) {
    // 说明需要调整采样率和声道数，这里需要做一些方便接下来调整的准备工作
    // 找到比wantedSpec.channels少但是最接近的声道数以及比wantedSpec.freq小但是最接近的采样率
    // 即使两个数组都很小，也用二分
    srIter = std::lower_bound(srAlternatives.begin(), srAlternatives.end(), wantedSR); // 找到的是第一个不小于wantedSR的值
    // 如果sr是首元素，说明wantedSR比最小采样率还小
    if (srIter == srAlternatives.begin()) throw ErrorDesc::from(ExceptionType::FFmpegSetFailed, "Invalid sample rate");
    chIter = std::lower_bound(chNumAlternatives.begin(), chNumAlternatives.end(), wantedChLayOut->nb_channels);
    // 如果ch是首元素，说明wantedChLayOut->nb_channels比最小声道数还小
    if (chIter == chNumAlternatives.begin()) throw ErrorDesc::from(ExceptionType::FFmpegSetFailed, "Invalid channel number");
  }
  while (!audioDeviceId) {
    // 说明打开失败，尝试调整采样率和声道数
    // 优先调整声道数
    if (chIter != chNumAlternatives.begin()) {
      // 说明声道数可以更小
      // 将srIter指向前一个元素，变更其意义为更小的采样率
      --chIter;
      wantedSpec.channels = *chIter;
      // FFUtil::relayoutChannel(wantedChLayOut, wantedSpec.channels); 不需要调整，因为wantedSpec.channels只需要数量，这里最后再打开设备后会检查
    } else {
      // 如果声道数已经调整到最小，就调整采样率, 但是对于声道数仍然仅仅使用chosenChNum
      if (srIter != srAlternatives.begin()) {
        // 将chIter指向前一个元素，变更其意义为更小的声道数
        --srIter;
        wantedSpec.freq = *srIter;
        wantedSpec.channels = chosenChNum;
        // samples也要重新计算
        wantedSpec.samples = FFUtil::getSamplesPerSec(wantedSpec.freq, wantedCallsPerSec, maxCallsPerSec, minSamplesContainedIn1Call);
      } else {
        // 如果采样率也调整到最小，就返回错误
        throw ErrorDesc::from(ExceptionType::FFmpegSetFailed, "Invalid sample rate and channel number. All tries failed");
      }
    }
    // 重新打开设备
    audioDeviceId = SDL_OpenAudioDevice(
      nullptr,
      0,
      &wantedSpec,
      &obtainedSpec,
      SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE
    );
  }
  // 打开成功
  // 检查得到的采样率是否符合要求
  if (obtainedSpec.freq != mustBeFormat) {
    // 释放设备
    SDL_CloseAudioDevice(audioDeviceId);
    throw ErrorDesc::from(ExceptionType::FFmpegSetFailed, "Invalid sample rate");
  }
  // 如果声道数与wantedChLayOut不同，是可以接受的
  if (obtainedSpec.channels != wantedChLayOut->nb_channels) {
    FFUtil::relayoutChannel(wantedChLayOut, obtainedSpec.channels);
    // 若调整后的声道数仍然不符合要求，抛出异常
    if (wantedChLayOut->nb_channels != obtainedSpec.channels || wantedChLayOut->order != AV_CHANNEL_ORDER_NATIVE) {
      // 释放设备
      SDL_CloseAudioDevice(audioDeviceId);
      throw ErrorDesc::from(ExceptionType::FFmpegSetFailed, "Invalid channel number set to SDL");
    }
  }
  // 记录下来,copySDLAudioSpecToAudioParams可能抛出异常，但是它如果抛出，也不是我们能处理得了
  return copySDLAudioSpecToAudioParams(obtainedSpec, wantedChLayOut);
}
