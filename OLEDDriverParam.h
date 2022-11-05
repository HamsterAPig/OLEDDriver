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
  #define OLED_COMMAND_BUFFER_LENGTH 32  // OLED 命令存储Buffer长度
#endif
#ifdef __USING_SSD1306
const uint8_t __oled_init_param[] = {0xae, 0x20, 0x00, 0xb0, 0xc8, 0x00, 0x10, 0x40, 0x81, 0xff,
                                     0xa1, 0xa6, 0xa8, 0x3f, 0xa4, 0xd3, 0x00, 0xd5, 0xf0, 0xd9,
                                     0x22, 0xda, 0x12, 0xdb, 0x20, 0x8d, 0x14, 0xaf};
const uint8_t __oled_write_command_param[] = {0xb0, 0x00, 0x10};
const uint8_t __oled_on_pararm[] = {0x8d, 0x14, 0xaf};
const uint8_t __oled_off_param[] = {0x8d, 0x10, 0xae};
#endif
#endif  // OLEDDRIVERPARAM_H
