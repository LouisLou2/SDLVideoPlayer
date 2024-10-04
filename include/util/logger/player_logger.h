//
// Created by leo on 24-10-2.
//

#ifndef PLAYER_LOGGER_H
#define PLAYER_LOGGER_H

#include <iostream>

#include "entity/error_desc.h"

class PlayerLogger { // 突然觉得不需要继承Logger基类，因为这个类只是一个工具类， 感觉继承基类没必要，Logger基类先放那里吧不管了
public:
  static inline void log(const ErrorDesc& detail);
  static inline void log(LogLevel level, const std::string& desc);
};

inline void PlayerLogger::log(const ErrorDesc& detail) {
  // 这里应该使用spdlog库来打印日志, 但是如果现在就将库引入, 会导致编译太慢，所以现在测试的时候先用标准输出吧
  std::cout << "LogLevel: " << static_cast<int>(detail.getLevel()) << ", ErrorDesc: " << detail.what() << std::endl;
}

inline void PlayerLogger::log(LogLevel level, const std::string& desc) {
  // 这里应该使用spdlog库来打印日志, 但是如果现在就将库引入, 会导致编译太慢，所以现在测试的时候先用标准输出吧
  std::cout << "LogLevel: " << static_cast<int>(level) << ". ErrorDesc: " << desc << std::endl;
}

#endif //PLAYER_LOGGER_H
