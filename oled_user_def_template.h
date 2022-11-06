/**
 * @Description 用户定义头文件
 * @Author jinming xi
 * @Date 2022/11/6
 * @Project OLED 驱动程序用户自定义文件
 */

#ifndef OLED_USER_DEF_H
#define OLED_USER_DEF_H

// #define __USING_SSD1306
#define __USING_SH1106

#define OLED_ENABLE_WRAP
#define OLED_USING_DMA_TRANSMIT
#define OLED_TRANSMIT_MODE_SPI

#ifdef __USING_SH1106
#  define OLED_USING_PAGE_MODE
#  define OLED_ENABLE_PIN_CS
#  define OLED_ENABLE_PIN_DC
#endif

#endif  // OLED_USER_DEF_H
