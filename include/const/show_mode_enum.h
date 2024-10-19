//
// Created by leo on 24-10-9.
//

#ifndef SHOW_MODE_ENUM_H
#define SHOW_MODE_ENUM_H
#include <cstdint>
//仅仅对于音频存在的情况下，才有意义，分别是显示视频、显示波形、显示快速离散余弦变换
/* 下面枚举值二进制的第4位表示是否显示音频，
 * 第5位表示是否显示图片，
 * 第6位表示是否显示字幕，
 * 剩下两位可表示四个值，表示音频的可视化方式，目前用到00和01，分别表示波形和快速离散余弦变换
 */
// 注意，决不允许定义既没有音频也没有视频的情况以及视频和字幕不同时存在的情况，要不会引起极大的混乱
enum class ShowModeEnum: uint8_t {
  All = 0b00011100, // 显示所有
  OnlyPic = 0b00001000, // 仅仅显示图片
  AudioAndPic = 0b00011000, // 显示音频和图片,忽略字幕
  PicAndSub = 0b00001100, // 显示图片和字幕,忽略音频
  // AudioAndSub = 0b00010100, // 显示音频和字幕,忽略图片
  AudioWithWaves = 0b00010000, // 显示音频和波形,忽略图片
  AudioWithRDFT = 0b00010001, // 显示音频和快速离散余弦变换,忽略图片
};

// 注意和ShowModeEnum后两比特位对应
enum class AudioVisualForm : uint8_t{
  Waves = 0,
  RDFT = 1,
};

class ShowModeUtil {
public:
  static bool isEnableAudio(ShowModeEnum showMode) {
    return static_cast<uint8_t>(showMode) & 0b00010000;
  }
  static bool isEnableVideo(ShowModeEnum showMode) {
    return static_cast<uint8_t>(showMode) & 0b00001000;
  }
  static bool isEnableSub(ShowModeEnum showMode) {
    return static_cast<uint8_t>(showMode) & 0b00000100;
  }
  // inline static ShowModeEnum getShowModeEnum(bool enableAud, bool enableVid, bool enableSub, AudioVisualForm audVisForm = AudioVisualForm::Waves);
};

// inline ShowModeEnum ShowModeUtil::getShowModeEnum(bool enableAud, bool enableVid, bool enableSub, AudioVisualForm audVisForm) {
//     uint8_t res = 0;
//     if (enableAud) {
//         res |= 0b00010000;
//     }
//     if (enableVid) {
//         res |= 0b00001000;
//     }
//     if (enableSub) {
//         res |= 0b00000100;
//     }
//     res |= static_cast<uint8_t>(audVisForm);
//     return static_cast<ShowModeEnum>(res);
// }

#endif //SHOW_MODE_ENUM_H
