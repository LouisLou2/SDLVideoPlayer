//
// Created by leo on 24-10-9.
//

#ifndef SCREEN_INFO_H
#define SCREEN_INFO_H
#include <cstdint>

class ScreenInfo {
  uint32_t fmt;
  uint32_t w;
  uint32_t h;
  uint16_t refreshRate;
public:
  ScreenInfo(uint32_t fmt, uint32_t w, uint32_t h, uint16_t refreshRate): fmt(fmt), w(w), h(h), refreshRate(refreshRate) {}
  [[nodiscard]] uint32_t getFmt() const{
    return fmt;
  }
  [[nodiscard]] uint32_t getW() const{
    return w;
  }
  [[nodiscard]] uint32_t getH() const{
    return h;
  }
  [[nodiscard]] uint16_t getRefreshRate() const{
    return refreshRate;
  }
  void setFmt(uint32_t fmt){
    this->fmt = fmt;
  }
  void setW(uint32_t w){
    this->w = w;
  }
  void setH(uint32_t h){
    this->h = h;
  }
  void setRefreshRate(uint16_t refreshRate){
    this->refreshRate = refreshRate;
  }
  ~ScreenInfo() = default;
};



#endif //SCREEN_INFO_H
