//
// Created by leo on 24-10-9.
//
#include "player/sdl_video_player/sdl_video_player.h"


#ifdef __cplusplus
extern "C" {
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
}
#else
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#endif


SDLVideoPlayer::SDLVideoPlayer(
  const std::string& video_path,
  const std::optional<SDLVidPlayerSettings>& setting
):
settings(
  setting.value_or(
    SDLVidPlayerSettings(
      SDLVideoPlayer::programName + " " + video_path,
      true,
      false,
      false,
      false,
      ShowModeEnum::None,
      false,
      DEF_WIN_WIDTH,
      DEF_WIN_HEIGHT
      )
    )
),
cacheCollection(
  PIC_CAP_MAX, true,
  SAMPLE_CAP_MAX, false,
  SUB_CAP_MAX, false
)
{
  videoInfo.originalUrl = video_path;
}

void SDLVideoPlayer::initAv() {
  avdevice_register_all(); // 高版本的ffmpeg这一步是可选的
  // 初始化网络
  if (settings.enableNetwork) avformat_network_init();
}

void SDLVideoPlayer::initSDL() {
  uint32_t flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
  if (settings.disableAud) flags &= ~SDL_INIT_AUDIO;
  if (settings.disableVid) flags &= ~SDL_INIT_VIDEO;
  if (SDL_Init(flags)) {
    // 初始化失败
    throw ErrorDesc::from(ExceptionType::SDLInitFailed, SDL_GetError());
  }
  initHardWareAfterSDL();
  // 使SDL不处理这些事件，以便我们自己处理
  SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
  SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

  if (settings.disableVid) return;

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
      PlayerLogger::log(LogLevel::Info, std::string("Renderer Initialized: ") + rendererInfo.name);
  }
  // 检查一系列创建是否成功
  if (!window || !renderer || !rendererInfo.num_texture_formats) {
    throw ErrorDesc::from(ExceptionType::SDLComponentInitFailed, SDL_GetError());
  }
}


void SDLVideoPlayer::initHardWareAfterSDL() {
  uint16_t numDisplays = SDL_GetNumVideoDisplays();
  if (numDisplays < 1) {
    throw ErrorDesc::from(ExceptionType::UnsupportedHardware, "No display found By SDL");
  }
  std::vector<ScreenInfo> screenInfos;
  SDL_DisplayMode mode;
  for (uint16_t i = 0; i < numDisplays; ++i) {
    if (SDL_GetCurrentDisplayMode(i, &mode) == 0) {
      screenInfos.emplace_back(i, mode.w, mode.h, mode.refresh_rate);
    }
  }
  if (screenInfos.empty()) {
    throw ErrorDesc::from(ExceptionType::UnsupportedHardware, "Can't get any information of displays");
  }
  SDLVidPlayerSettings::setScreenInfos(std::move(screenInfos));
  if (settings.useDisplayIndex >= SDLVidPlayerSettings::screenInfo.size()) {
    throw ErrorDesc::from(ExceptionType::InvalidArgument, "Invalid display index");
  }
}