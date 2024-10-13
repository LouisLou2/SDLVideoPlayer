//
// Created by leo on 24-10-4.
//

#include "player/sdl_video_player/sdl_video_basic_info.h"
#include <util/url_util.h>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

VideoBasicInfo::VideoBasicInfo():streamIndex({-1,-1,-1,-1,-1}) {

}

std::optional<ErrorDesc> VideoBasicInfo::checkFileAndSetBasic() {
  // 检查文件是否为本地文件
  protocol = UrlUtil::getProtocolType(originalUrl);
  // 如果是本地文件，检查文件是否存在
  if (protocol == StreamProtocol::File) {
    // 检查文件是否存在
    if (!fs::is_regular_file(originalUrl)) {
      return ErrorDesc::from(ExceptionType::ResourceNotFound, std::string("Can't find an regular file named: ") + originalUrl);
    }
    // 检查文件是否过小
    if (fs::file_size(originalUrl) < MIN_STREAM_SIZE) {
      return ErrorDesc::from(ExceptionType::ResourceNotFound, std::string("File is too small: ") + originalUrl);
    }
    // 打开文件检查可读性
    std::ifstream videoInFile(originalUrl, std::ios::in | std::ios::binary);
    if (!videoInFile.is_open()) {
      return ErrorDesc::from(ExceptionType::FileUnreadable, std::string("Can't open file: ") + originalUrl);
    }else {
      // 关闭文件
      videoInFile.close();
    }
  }
  return std::nullopt;
}
