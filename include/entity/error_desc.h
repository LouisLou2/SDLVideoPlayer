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
  /*
   * 构造函数私有化，只能通过from方法创建
   */
  ErrorDesc(LogLevel level, ExceptionType type, std::string_view typeStr, auto&& desc);
public:
  // 注意默认构造函数创建的对象不具有任何意义，只是为了方便需要创建一个空对象的情况
  ErrorDesc() = default;
  [[nodiscard]] LogLevel getLevel() const { return level; }
  [[nodiscard]] const char* what() const noexcept override { return formattedMsg.c_str(); };
  // 移动构造函数
  ErrorDesc(ErrorDesc&&) noexcept;
  // 移动赋值运算符
  ErrorDesc& operator=(ErrorDesc&&) noexcept;
  // 拷贝构造函数
  ErrorDesc(const ErrorDesc&) noexcept;
  // 拷贝赋值运算符
  ErrorDesc& operator=(const ErrorDesc&) noexcept;
  /*
   * 当实参desc是string的右值引用，右值，const char*或者char*时，auto&&会推导为...
   */
  void reset(ExceptionType ty, auto&& desc);
  [[nodiscard]] static ErrorDesc from(ExceptionType ty, auto&& desc);
};

ErrorDesc::ErrorDesc(LogLevel level, ExceptionType type, std::string_view typeStr, auto&& desc) :
  level(level),
  type(type),
  typeStr(typeStr),
  desc(std::forward<decltype(desc)>(desc)),
  formattedMsg(std::format("[ Code: {} ] {}: {}", ExceptionTypeUtil::typeCode(this->type), this->typeStr, this->desc)){}

void ErrorDesc::reset(ExceptionType ty, auto&& desc) {
  type = ty;
  typeStr = ExceptionTypeUtil::toStr(ty);
  this->desc = std::forward<decltype(desc)>(desc);
  formattedMsg = std::format("[ Code: {} ] {}: {}", ExceptionTypeUtil::typeCode(this->type), this->typeStr, this->desc);
}

ErrorDesc ErrorDesc::from(ExceptionType ty, auto&& desc) {
  return ErrorDesc(ExceptionTypeUtil::getLogLevel(ty), ty, ExceptionTypeUtil::toStr(ty), std::forward<decltype(desc)>(desc));
}

inline ErrorDesc::ErrorDesc(ErrorDesc&& other) noexcept :
  level(other.level),
  type(other.type),
  typeStr(other.typeStr), // 这个直接复制string_view是即可，他指向的字符串是静态的
  desc(std::move(other.desc)),
  formattedMsg(std::move(other.formattedMsg)){}

inline ErrorDesc& ErrorDesc::operator=(ErrorDesc&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  level = other.level;
  type = other.type;
  typeStr = other.typeStr;
  desc = std::move(other.desc);
  formattedMsg = std::move(other.formattedMsg);
  return *this;
}

inline ErrorDesc::ErrorDesc(const ErrorDesc& other) noexcept :
  level(other.level),
  type(other.type),
  typeStr(other.typeStr),
  desc(other.desc),
  formattedMsg(other.formattedMsg){}

inline ErrorDesc& ErrorDesc::operator=(const ErrorDesc& other) noexcept{
  if (this == &other) {
    return *this;
  }
  level = other.level;
  type = other.type;
  typeStr = other.typeStr;
  desc = other.desc;
  formattedMsg = other.formattedMsg;
  return *this;
}

#endif //ERROR_DESC_H
