//
// Created by leo on 24-10-6.
//

#ifndef THREAD_COORDINATOR_H
#define THREAD_COORDINATOR_H
#include <condition_variable>

struct SDLThreadCoordinator {
  friend class SDLVideoPlayer;
private:
  std::condition_variable continueReadCV;
  std::thread readThread;
};

#endif //THREAD_COORDINATOR_H
