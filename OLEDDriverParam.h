/**
 * @Description OLED 芯片层参数配置文件
 * @Author jinming xi
 * @Date 2022/11/5
 * @Project F401CCU6
 */

#ifndef OLEDDRIVERPARAM_H
#define OLEDDRIVERPARAM_H
#include <stdint.h>
#ifndef OLED_COMMAND_BUFFER_LENGTH
#  define OLED_COMMAND_BUFFER_LENGTH 32  // OLED 命令存储Buffer长度
#endif
#ifdef __USING_SSD1306
#  ifdef OLED_USING_PAGE_MODE
const uint8_t __oled_init_param[] = {0xae,        // 关闭显示屏
                                     0xc8,        // 0x09 上下反置，0xc8正常
                                     0xa1,        // 0xa0左右反置，0xa1正常
                                     0xa4,        // 全局显示开启,0xa4正常,0xa5无视GRAM内容点亮全屏
                                     0xa6,        // 0xa7反色显示
                                     0xb0,        // 设置起始页地址，只对页寻址模式有效
                                     0x40,        // Set display RAM display start line register from 0~63
                                     0x20, 0x10,  // 设置为Page寻址模式
                                     0xd5, 0x80,  // 设置时钟分频因子，振荡频率
                                     0x81, 0x7f,  // 设置对比度
                                     0x8d, 0x14,  // 打开电荷泵
                                     0x00, 0x10,  // 设置起始列地址和终止列地址
                                     0xd3, 0x00,  // 设置垂直偏移地址
                                     0xa8, 0x3f,  // Set Mux ratio to N+1 MUX
                                     0xda, 0x12,  // Set COM Pins Hardware configuration
                                     0xd9, 0x22,  // Set Pre-Charge Period
                                     0xdb, 0x10,  // Set V COMH Deselect Level
                                     0x22, 0x00, 0x07,  // 设置页的起始地址和结束地址
                                     0xaf};
#  else
const uint8_t __oled_init_param[] = {0xae,        // 关闭显示屏
                                     0xc8,        // 0x09 上下反置，0xc8正常
                                     0xa1,        // 0xa0左右反置，0xa1正常
                                     0xa4,        // 全局显示开启,0xa4正常,0xa5无视GRAM内容点亮全屏
                                     0xa6,        // 0xa7反色显示
                                     0xb0,        // 设置起始页地址，只对页寻址模式有效
                                     0x40,        // Set display RAM display start line register from 0~63
                                     0x20, 0x00,  // 设置为水平寻址模式
                                     0xd5, 0x80,  // 设置时钟分频因子，振荡频率
                                     0x81, 0x7f,  // 设置对比度
                                     0x8d, 0x14,  // 打开电荷泵
                                     0x00, 0x10,  // 设置起始列地址和终止列地址
                                     0xd3, 0x00,  // 设置垂直偏移地址
                                     0xa8, 0x3f,  // Set Mux ratio to N+1 MUX
                                     0xda, 0x12,  // Set COM Pins Hardware configuration
                                     0xd9, 0x22,  // Set Pre-Charge Period
                                     0xdb, 0x10,  // Set V COMH Deselect Level
                                     0x22, 0x00, 0x07,  // 设置页的起始地址和结束地址
                                     0xaf};
#  endif
const uint8_t __oled_on_pararm[] = {0x8d, 0x14, 0xaf};
const uint8_t __oled_off_param[] = {0x8d, 0x10, 0xae};
#endif
#ifdef __USING_SH1106
#  ifdef OLED_USING_PAGE_MODE
const uint8_t __oled_init_param[] = {0xae,        // 关闭显示屏
                                     0xc8,        // 0x09 上下反置，0xc8正常
                                     0xa1,        // 0xa0左右反置，0xa1正常
                                     0xa4,        // 全局显示开启,0xa4正常,0xa5无视GRAM内容点亮全屏
                                     0xa6,        // 0xa7反色显示
                                     0xb0,        // 设置起始页地址，只对页寻址模式有效
                                     0x40,        // Set display RAM display start line register from 0~63
                                     0x20, 0x10,  // 设置为Page寻址模式
                                     0xd5, 0x80,  // 设置时钟分频因子，振荡频率
                                     0x81, 0xff,  // 设置对比度
                                     0x8d, 0x14,  // 打开电荷泵
                                     0x00, 0x10,  // 设置起始列地址和终止列地址
                                     0xd3, 0x00,  // 设置垂直偏移地址
                                     0xa8, 0x3f,  // Set Mux ratio to N+1 MUX
                                     0xda, 0x12,  // Set COM Pins Hardware configuration
                                     0xd9, 0x22,  // Set Pre-Charge Period
                                     0xdb, 0x10,  // Set V COMH Deselect Level
                                     0x22, 0x00, 0x07,  // 设置页的起始地址和结束地址
                                     0xaf};// 这里这个参数得偏移一位，因为SH1106是132的被当成了128来用
const uint8_t __oled_write_param[] = {0xb0, 0x02, 0x10};
#  else
const uint8_t __oled_init_param[] = {0xae,        // 关闭显示屏
                                     0xc8,        // 0x09 上下反置，0xc8正常
                                     0xa1,        // 0xa0左右反置，0xa1正常
                                     0xa4,        // 全局显示开启,0xa4正常,0xa5无视GRAM内容点亮全屏
                                     0xa6,        // 0xa7反色显示
                                     0xb0,        // 设置起始页地址，只对页寻址模式有效
                                     0x40,        // Set display RAM display start line register from 0~63
                                     0x20, 0x00,  // 设置为水平寻址模式
                                     0xd5, 0x80,  // 设置时钟分频因子，振荡频率
                                     0x81, 0x7f,  // 设置对比度
                                     0x8d, 0x14,  // 打开电荷泵
                                     0x00, 0x10,  // 设置起始列地址和终止列地址
                                     0xd3, 0x00,  // 设置垂直偏移地址
                                     0xa8, 0x3f,  // Set Mux ratio to N+1 MUX
                                     0xda, 0x12,  // Set COM Pins Hardware configuration
                                     0xd9, 0x22,  // Set Pre-Charge Period
                                     0xdb, 0x10,  // Set V COMH Deselect Level
                                     0x22, 0x00, 0x07,  // 设置页的起始地址和结束地址
                                     0xaf};
#  endif
const uint8_t __oled_on_pararm[] = {0x8d, 0x14, 0xaf};
const uint8_t __oled_off_param[] = {0x8d, 0x10, 0xae};
#endif
#endif  // OLEDDRIVERPARAM_H
