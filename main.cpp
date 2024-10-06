#include <chrono>
#include <player/sdl_video_player.h>
#include <player/video_player.h>

#include "entity/error_desc.h"
#include "util/logger/player_logger.h"

void testio() {
  // auto start = std::chrono::high_resolution_clock::now();
  // for (uint32_t i = 0; i<1000;++i) {
  //   ErrorDesc desc = ErrorDesc::from(ExceptionType::UnsupportedFormat, "Unknown error");
  //   PlayerLogger::log(desc);
  // }
  // auto end = std::chrono::high_resolution_clock::now();
  // // ms
  // std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

  std::array<long, 3> arr{};
  auto start = std::chrono::high_resolution_clock::now();
  for (uint32_t i = 0; i<10000;++i) {
    std::cout << std::format("Hello, {}, {}!\n", 18767675, "ew");
  }
  auto end = std::chrono::high_resolution_clock::now();
  arr[0]=std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  start = std::chrono::high_resolution_clock::now();
  for (uint32_t i = 0; i<10000;++i) {
    std::cout << "Hello, " << 18767675 << ", " << "ew!\n";
  }
  end = std::chrono::high_resolution_clock::now();
  arr[1]=std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  start = std::chrono::high_resolution_clock::now();
  for (uint32_t i = 0; i<10000;++i) {
    std::cout << std::string("Hello, ") + std::to_string(18767675) + ", ew!\n";
  }
  end = std::chrono::high_resolution_clock::now();
  arr[2]=std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  for (auto& i : arr) {
    std::cout << i << std::endl;
  }
}


int main() {
  // try {
  //   std::cout << "Throwing exception now..." << std::endl;
  //   throw ErrorDesc::from(ExceptionType::UnsupportedFormat, std::string("Unknown error"));
  // } catch (std::exception& e) {
  //   PlayerLogger::log(LogLevel::Critical,e.what());
  // }
  VideoPlayer&& player = SDLVideoPlayer("/home/leo/Media/medias/bejeweled_20-23.mp4",std::nullopt);
  player.play();
  return 0;
}
