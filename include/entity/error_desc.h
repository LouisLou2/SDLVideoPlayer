//
// Created by leo on 24-9-30.
//

#ifndef ERROR_DESC_H
#define ERROR_DESC_H

#include <string_view>
#include <string>
#include <format>
#include "const/error_type.h"

class ErrorDesc : public std::exception {
  LogLevel level;
  ExceptionType type;
  std::string_view typeStr;
  std::string desc;
  std::string formattedMsg;
public:
  ErrorDesc(LogLevel level, ExceptionType type, std::string_view typeStr, const std::string& desc) :
  level(level),
  type(type),
  typeStr(typeStr),
  desc(desc) {
    // 会触发移动赋值运算符
    formattedMsg = std::format("[ Code: {} ] {}: {}", ExceptionTypeUtil::typeCode(type), typeStr, desc);
  }
  [[nodiscard]] LogLevel getLevel() const { return level; }
  [[nodiscard]] const char* what() const noexcept override { return formattedMsg.c_str(); };
  [[nodiscard]] inline static ErrorDesc from(ExceptionType ty, const std::string& desc);
};

inline ErrorDesc ErrorDesc::from(ExceptionType ty, const std::string& desc) {
  return ErrorDesc{ ExceptionTypeUtil::getLogLevel(ty), ty, ExceptionTypeUtil::toStr(ty), desc };
}

#endif //ERROR_DESC_H
