//
// Created by leo on 24-10-2.
//

#ifndef LOGGER_H
#define LOGGER_H
#include "entity/error_desc.h"

class Logger {
public:
  virtual void log(const ErrorDesc& detail) = 0;
  virtual void log(LogLevel level, const std::string& desc) = 0;
  virtual ~Logger() = default;
};



#endif //LOGGER_H
