/**
 * @Description OLED驱动程序
 * @Author jinming xi
 * @Date 2022/11/3
 * @Project OLEDDriver
 */
#include "OLEDDriver.h"
#include "codetable.h"

/**
 * OLED 写命令函数，作用于仅限于本文件
 * @param pcmd 命令数组指针
 * @param total 命令长度
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_WriteCmd(uint8_t *pcmd, uint16_t total);

/**
 * @breif 定义两个数组
 * @parm g_oled_buffer作为图形缓存
 * @parm g_command_buffer作为命令缓存
 */
uint8_t g_oled_buffer[OLED_PAGE_SIZE][OLED_PIX_WIDTH];
uint8_t g_command_buffer[OLED_COMMAND_BUFFER_LENGTH];

/**
 * OLED初始化函数
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_Init(void)
{
  const uint8_t _ssd1306_init_parm[] = {0xae, 0x20, 0x00, 0xb0, 0xc8, 0x00, 0x10, 0x40, 0x81, 0xff,
                                        0xa1, 0xa6, 0xa8, 0x3f, 0xa4, 0xd3, 0x00, 0xd5, 0xf0, 0xd9,
                                        0x22, 0xda, 0x12, 0xdb, 0x20, 0x8d, 0x14, 0xaf};
  return OLED_WriteCmd((uint8_t *)_ssd1306_init_parm, 28);
}

/**
 * 将g_oled_buffer里面的内容更新到屏幕中
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_Reflush_GSRAM()
{
  OLED_StatusTypeDef status;
  const uint8_t _ssd1306_write_command_parm[] = {0xb0, 0x00, 0x10};
  status = OLED_WriteCmd((uint8_t *)_ssd1306_write_command_parm, 3);
  if (status != OLED_OK) return status;
  status = OLED_I2C_Transmit(OLED_PHY_ADDRESS, 0x40, (uint8_t *)g_oled_buffer, OLED_PIX_WIDTH * OLED_PAGE_SIZE);
  return status;
}

/**
 * 点亮某一个点或者点灭某一个点
 * @param x 横坐标 0~横向像素 - 1
 * @param y 纵坐标 0~纵向像素 - 1
 * @param state 点的状态，点亮为1，点灭为0
 * @return OLED Status
 */
#define SET_BIT(REG, BIT) ((REG) |= (BIT))
#define CLEAR_BIT(x, bit) (x &= ~(1 << bit)) /* 清零第bit位 */
OLED_StatusTypeDef OLED_SetPoint(uint8_t x, uint8_t y, uint8_t state)
{
  // 检查输入参数.超出范围自动返回错误
  if (x >= OLED_PIX_WIDTH || y >= OLED_PIX_HEIGHT) return OLED_OUT_RANGE;

  uint8_t fill_data = y % 8;
  uint8_t old_data = g_oled_buffer[(y - fill_data) / 8][x];
  if (0 == state)
    g_oled_buffer[(y - fill_data) / 8][x] = CLEAR_BIT(old_data, fill_data);
  else
    SET_BIT(g_oled_buffer[(y - fill_data) / 8][x], state << fill_data);

  return OLED_OK;
}

/**
 * 将g_oled_buffer里面的内容更新到屏幕中
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_Fill(uint8_t state)
{
  OLED_StatusTypeDef status;
  memset(g_oled_buffer, state, sizeof(uint8_t) * OLED_PIX_WIDTH * OLED_PAGE_SIZE);
  status = OLED_Reflush_GSRAM();
  return status;
}

/**
 * 清屏
 * @note 执行此函数会立即将修改同步到屏幕当中
 * @return
 */
OLED_StatusTypeDef OLED_Clear() { return OLED_Fill(0x00); }

/**
 * 字符串显示函数
 * @param x 横坐标 0 ~ 横向像素 - 1
 * @param y 纵坐标 0 ~ 纵向页数 - 1
 * @param pstr 字符串指针
 * @param text_size 显示的大小
 * @return
 */
OLED_StatusTypeDef OLED_ShowStr(uint8_t x, uint8_t y, uint8_t *pstr, uint8_t text_size)
{
  uint8_t pstr_index = 0;
  uint8_t charter = 0;
  switch (text_size) {
    case 1:
      while (pstr[pstr_index] != '\0') {
        charter = pstr[pstr_index] - 32;
        if (x > OLED_PIX_WIDTH - 2) {
#if OLED_ENABLE_WRAP
          x = 0;
          y++;
#else
          return OLED_OUT_RANGE;
#endif
        }
        for (int i = 0; i < 6; ++i) g_oled_buffer[y][x + i] = F6x8[charter][i];
        x += 6;
        pstr_index++;
      }
      break;
    case 2:
      while (pstr[pstr_index] != '\0') {
        charter = pstr[pstr_index] - 32;
        if (x > OLED_PIX_WIDTH - 8) {
#if OLED_ENABLE_WRAP
          x = 0;
          y += 2;
#else
          return OLED_OUT_RANGE;
#endif
        }
        for (int i = 0; i < 8; ++i) g_oled_buffer[y][x + i] = F8X16[charter * 16 + i];
        for (int i = 0; i < 8; ++i) g_oled_buffer[y + 1][x + i] = F8X16[charter * 16 + i + 8];
        x += 8;
        pstr_index++;
      }
      break;
  }
}

OLED_StatusTypeDef OLED_ON()
{
  const uint8_t _ssd1306_on_parm[] = {0x8d, 0x14, 0xaf};
  return OLED_WriteCmd((uint8_t *)_ssd1306_on_parm, 3);
}

OLED_StatusTypeDef OLED_OFF()
{
  const uint8_t _ssd1306_off_parm[] = {0x8d, 0x10, 0xae};
  return OLED_WriteCmd((uint8_t *)_ssd1306_off_parm, 3);
}

/**
 * OLED 写命令函数，作用于仅限于本文件
 * @param pcmd 命令数组指针
 * @param total 命令长度
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_WriteCmd(uint8_t *pcmd, uint16_t total)
{
  /**
   * @note 拷贝一份原有数据防止函数退出后，pcmd指向的地址失效
   * @note 进而导致DMA发送数据异常
   */
  for (int i = 0; i < total; ++i) g_command_buffer[i] = pcmd[i];
  OLED_StatusTypeDef status = OLED_I2C_Transmit(OLED_PHY_ADDRESS, 0x00, g_command_buffer, total);
  return status;
}
