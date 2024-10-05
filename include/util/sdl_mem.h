//
// Created by leo on 24-9-30.
//

#ifndef SDL_MEM_H
#define SDL_MEM_H
#ifdef __cplusplus
extern "C"{
#include <SDL2/SDL.h>
}
#else
#include <SDL2/SDL.h>
#endif


// SDL_Window 的自定义删除器
struct SDL_WindowDeleter {
  void operator()(SDL_Window* window) const {
    SDL_DestroyWindow(window);
  }
};
struct SDL_RendererDeleter {
  void operator()(SDL_Renderer* renderer) const {
    SDL_DestroyRenderer(renderer);
  }
};

struct SDL_TextureDeleter {
  void operator()(SDL_Texture* texture) const {
    SDL_DestroyTexture(texture);
  }
};

// struct SDL_ThreadDeleter {
//   void operator()(SDL_Thread* thread) const {
//     int status;
//     SDL_WaitThread(thread, &status);
//     printf("SDL_ThreadDeleter: status=%d\n", status);
//   }
// };
#endif //SDL_MEM_H
