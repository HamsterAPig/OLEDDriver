/**
 * @Description OLED驱动程序
 * @Author jinming xi
 * @Date 2022/11/3
 * @Project OLEDDriver
 */
#include "OLEDDriver.h"
#include "OLEDDriverParam.h"
#include "codetable.h"

#define CALC_NUM_LENGTH(x) sizeof(x) / sizeof(uint8_t)

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
#ifdef OLED_USING_DMA_TRANSMIT
uint8_t g_call_refresh_count = 0;
uint8_t gb_call_refresh = 0;
#endif

/**
 * OLED初始化函数
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_Init(void)
{
  return OLED_WriteCmd((uint8_t *)__oled_init_param, CALC_NUM_LENGTH(__oled_init_param));
}

/**
 * 将g_oled_buffer里面的内容更新到屏幕中
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_Refresh_GSRAM()
{
  OLED_Refresh_GSRAM_CallBefore();
  OLED_StatusTypeDef status;
  status = OLED_Transmit(OLED_PHY_ADDRESS, 0x40, (uint8_t *)g_oled_buffer, OLED_PIX_WIDTH * OLED_PAGE_SIZE, 1);
  OLED_Refresh_GSRAM_CallAfter();
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
OLED_StatusTypeDef OLED_setPoint(uint8_t x, uint8_t y, uint8_t state)
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
  status = OLED_Refresh_GSRAM();
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
#ifdef OLED_ENABLE_WRAP
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
#ifdef OLED_ENABLE_WRAP
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

OLED_StatusTypeDef OLED_ON() { return OLED_WriteCmd((uint8_t *)__oled_on_pararm, CALC_NUM_LENGTH(__oled_on_pararm)); }

OLED_StatusTypeDef OLED_OFF() { return OLED_WriteCmd((uint8_t *)__oled_off_param, CALC_NUM_LENGTH(__oled_off_param)); }

/**
 * OLED 写命令函数，作用于仅限于本文件
 * @param pcmd 命令数组指针
 * @param total 命令长度
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_WriteCmd(uint8_t *pcmd, uint16_t total)
{
  OLED_WriteCmd_CallBefore();
  /**
   * @note 拷贝一份原有数据防止函数退出后，pcmd指向的地址失效
   * @note 进而导致DMA发送数据异常
   */
  for (int i = 0; i < total; ++i) g_command_buffer[i] = pcmd[i];
  OLED_StatusTypeDef status = OLED_Transmit(OLED_PHY_ADDRESS, 0x00, g_command_buffer, total, 0);
  OLED_WriteCmd_CallAfter();
  return status;
}

__weak void OLED_Refresh_GSRAM_CallBefore() { return; }
__weak void OLED_Refresh_GSRAM_CallAfter() { return; }

__weak void OLED_WriteCmd_CallBefore() { return; }
__weak void OLED_WriteCmd_CallAfter() { return; }