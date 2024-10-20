//
// Created by leo on 24-10-13.
//

#ifndef SDL_DISPLAYER_H
#define SDL_DISPLAYER_H
#include "sdl_audio_decode_regin.h"
#include "sdl_presentation_form.h"
#include "entity/audio_params.h"
#include "entity/audio_param_dto.h"

#ifdef __cplusplus
extern "C" {
#include <SDL2/SDL.h>
}
#else
#include <SDL2/SDL.h>
#endif
#include <memory>
#include <optional>
#include <functional>
#include "entity/error_desc.h"
#include "util/mem/sdl_mem.h"
#include "sdl_vid_player_setting.h"

class SDLDisplayer {
  // static
  static constexpr std::array<uint8_t,4> chNumAlternatives = {1,2,4,6};
  static constexpr std::array<uint32_t,5> srAlternatives = {8000, 44100, 48000, 96000, 192000};
  static constexpr uint16_t minSamplesContainedIn1Call = 512;
  static constexpr uint16_t wantedCallsPerSec = 24;
  static constexpr uint16_t maxCallsPerSec = 35;
  static constexpr int32_t silentSample = 0;

  // non-static
  std::unique_ptr<SDL_Window, SDL_WindowDeleter> window;
  std::unique_ptr<SDL_Renderer, SDL_RendererDeleter> renderer;
  SDL_RendererInfo rendererInfo;

  SDL_AudioDeviceID audioDeviceId;
  uint32_t audioBufThreshold;
public:
  std::optional<ErrorDesc> initDisplayer(const SDLVidPlayerSettings& settings,const MediaPresentForm& presentForm);

  // /*
  //  * @brief 根据SDL_AudioSpec和声道布局设置音频参数，之所以这样设计，是因为SDL_AudioSpec与AudioParams的参数不完全一致
  //  * @param spec SDL_AudioSpec
  //  * @param ch_layout 声道布局
  //  * @param setTgtByDisplayDecision 用于设置目标参数的函数
  //  */
  // static void setParamBySDLAudioSpec(
  //   const SDL_AudioSpec& spec,
  //   const AVChannelLayout* ch_layout,
  //   uint32_t hardwareAudBufSize,
  //   const setTgtByDisplayDecisionFunc& setTgtByDisplayDecision
  // ) noexcept(false);

  static AudioParamsDTO copySDLAudioSpecToAudioParams(
    const SDL_AudioSpec& spec,
    const AVChannelLayout* ch_layout
  ) noexcept(false);

  /*
   * @brief 给出期望的音频参数，根据这个参数配置音频显示
   * @param callback 音频回调函数
   * @param wantedChLayOut 期望的声道布局
   * @param wantedSR 期望的采样率
   * @param mustBeFormat 必须的音频格式
   * @note 只要返回，就代表配置成功，否则抛出异常
   */
  std::pair<AudioParamsDTO,uint32_t> configAudioDisplay(
    SDL_AudioCallback callback,
    AVChannelLayout* wantedChLayOut,
    uint32_t wantedSR,
    SDL_AudioFormat mustBeFormat
  ) noexcept(false);
};

#endif //SDL_DISPLAYER_H
