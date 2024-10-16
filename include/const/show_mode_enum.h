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

#endif //SHOW_MODE_ENUM_H
