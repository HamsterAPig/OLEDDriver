/**
 * @Description OLED驱动程序
 * @Author jinming xi
 * @Date 2022/11/3
 * @Project OLEDDriver
 */

#ifndef OLEDDRIVER_H
#define OLEDDRIVER_H
#ifdef __cplusplus
extern "C" {
#endif  // C++ Support
#include <memory.h>
#include <stdint.h>

#define OLED_PHY_ADDRESS 0x78  // I2C 物理地址
#define OLED_PIX_WIDTH 128     // OLED屏幕横向像素
#define OLED_PIX_HEIGHT 64     // OLED屏幕纵向像素

#define OLED_PAGE_SIZE OLED_PIX_HEIGHT / 8  // OLED驱动存储页数

#ifdef OLED_NO_WAIT_TRANSMIT_PROCESS
void OLED_DelayMS(uint8_t ms);
#endif

/**
 * @brief 定义返回常量，方便调试的时候判断状态
 */
typedef enum {
  OLED_OK = 0x00U,
  OLED_ERROR = 0x01U,
  OLED_OUT_RANGE = 0x02U,
} OLED_StatusTypeDef;

/**
 * @brief 使用二维数组存储像素，相当于是画布
 * @brief 所有对其的修改都不会立即同步到OLED上面
 * @brief 需要调用刷新函数
 */
extern uint8_t g_oled_buffer[OLED_PAGE_SIZE][OLED_PIX_WIDTH];

/**
 * OLED初始化函数
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_Init(void);

/**
 * I2C传输函数，需要用户自定义
 * @param DevAddress 设备地址
 * @param MemAddress 内存地址
 * @param pData 数据指针
 * @param Size 数据长度
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_I2C_Transmit(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size);

/**
 * 将g_oled_buffer里面的内容更新到屏幕中
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_Reflush_GSRAM();

/**
 * 点亮某一个点或者点灭某一个点
 * @param x 横坐标 0~横向像素 - 1
 * @param y 纵坐标 0~纵向像素 - 1
 * @param state 点的状态，点亮为1，点灭为0
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_SetPoint(uint8_t x, uint8_t y, uint8_t state);

/**
 * 全屏填充
 * @note 执行此函数会立即将修改同步到屏幕当中
 * @param state 0是清屏,1是点亮
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_Fill(uint8_t state);

/**
 * 清屏
 * @note 执行此函数会立即将修改同步到屏幕当中
 * @return
 */
OLED_StatusTypeDef OLED_Clear();

/**
 * 字符串显示函数
 * @param x 横坐标 0 ~ 横向像素 - 1
 * @param y 纵坐标 0 ~ 纵向页数 - 1
 * @param pstr 字符串指针
 * @param text_size 显示的大小
 * @return
 */
OLED_StatusTypeDef OLED_ShowStr(uint8_t x, uint8_t y, uint8_t *pstr, uint8_t text_size);

OLED_StatusTypeDef OLED_ON();
OLED_StatusTypeDef OLED_OFF();

#ifdef __cplusplus
}
#endif  // C++ Support
#endif  // OLEDDRIVER_H
