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
  const LogLevel level;
  const ExceptionType type;
  const std::string_view typeStr;
  const std::string desc;
  std::string formattedMsg;
  /*
   * 构造函数私有化，只能通过from方法创建
   */
  ErrorDesc(LogLevel level, ExceptionType type, std::string_view typeStr, auto&& desc);
public:
  [[nodiscard]] LogLevel getLevel() const { return level; }
  [[nodiscard]] const char* what() const noexcept override { return formattedMsg.c_str(); };
  /*
   * 当实参desc是string的右值引用，右值，const char*或者char*时，auto&&会推导为...
   */
  [[nodiscard]] inline static ErrorDesc from(ExceptionType ty, auto&& desc);
};

inline ErrorDesc::ErrorDesc(LogLevel level, ExceptionType type, std::string_view typeStr, auto&& desc) :
  level(level),
  type(type),
  typeStr(typeStr),
  desc(std::forward<decltype(desc)>(desc)),
  formattedMsg(std::format("[ Code: {} ] {}: {}", ExceptionTypeUtil::typeCode(this->type), this->typeStr, this->desc)){}
  ErrorDesc ErrorDesc::from(ExceptionType ty, auto&& desc) {
  return ErrorDesc(ExceptionTypeUtil::getLogLevel(ty), ty, ExceptionTypeUtil::toStr(ty), std::forward<decltype(desc)>(desc));
}

#endif //ERROR_DESC_H
