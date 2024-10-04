//
// Created by leo on 24-9-29.
//
#include "player/sdl_video_player.h"
#include "entity/error_desc.h"
#include "util/url_util.h"
#include <filesystem>
#include <optional>

#ifdef __cplusplus
extern "C" {
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
}
#else
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#endif

#define MIN_STREAM_SIZE 1024
#define DEF_WIN_WIDTH 640
#define DEF_WIN_HEIGHT 480

namespace fs = std::filesystem;


SDLVideoPlayer::SDLVideoPlayer(
  const std::string& video_path,
  const std::optional<PlayerSettings>& setting
):settings(
  setting.value_or(
    PlayerSettings(
      SDLVideoPlayer::programName,
      true,
      false,
      false,
      false,
      false,
      DEF_WIN_WIDTH,
      DEF_WIN_HEIGHT
      )
    )
){
  videoInfo.filename = video_path;
}


void SDLVideoPlayer::initAv(){
  avdevice_register_all(); // 高版本的ffmpeg这一步是可选的
  // 初始化网络
  if (settings.enableNetwork) avformat_network_init();
}

void SDLVideoPlayer::initSDL(){
  uint32_t flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
  if (settings.disableAud) flags &= ~SDL_INIT_AUDIO;
  if (settings.disableVid) flags &= ~SDL_INIT_VIDEO;
  if (SDL_Init(flags)) {
    // 初始化失败
    throw ErrorDesc::from(ExceptionType::SDLInitFailed, SDL_GetError());
  }
  // 使SDL不处理这些事件，以便我们自己处理
  SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
  SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

  if (!settings.disableVid) {
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
  }

}

void SDLVideoPlayer::play() {
  // 检查文件是否为本地文件
  videoInfo.protocol = UrlUtil::getProtocolType(videoInfo.filename);
  // 如果是本地文件，检查文件是否存在
  if (videoInfo.protocol == StreamProtocol::File) {
    // 检查文件是否存在
    if (!fs::is_regular_file(videoInfo.filename)) {
      throw ErrorDesc::from(ExceptionType::ResourceNotFound, std::string("Can't find an regular file named: ") + videoInfo.filename);
    }
    // 检查文件是否过小
    if (fs::file_size(videoInfo.filename) < MIN_STREAM_SIZE) {
      throw ErrorDesc::from(ExceptionType::ResourceNotFound, std::string("File is too small: ") + videoInfo.filename);
    }
    // 打开文件检查可读性
  }
  // 等到play函数调用时才初始化
  initAv();
  initSDL();
}
