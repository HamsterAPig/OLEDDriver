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
#ifndef OLED_USING_PAGE_MODE
OLED_StatusTypeDef OLED_Refresh_GSRAM()
{
  OLED_StatusTypeDef status;
#  ifdef OLED_NO_WAIT_TRANSMIT_PROCESS
  OLED_DelayMS(1);
#  endif
#  ifdef OLED_TRANSMIT_MODE_SPI
  OLED_setGPIO_CS(0);
#    ifdef OLED_TRANSMIT_MODE_SPI
  OLED_setGPIO_DC(1);
#    endif
#  endif
  status = OLED_Transmit(OLED_PHY_ADDRESS, 0x40, (uint8_t *)g_oled_buffer, OLED_PIX_WIDTH * OLED_PAGE_SIZE);
#  ifdef OLED_TRANSMIT_MODE_SPI
  OLED_setGPIO_CS(1);
#  endif
  return status;
}
#else
OLED_StatusTypeDef OLED_Refresh_GSRAM()
{
#  ifdef OLED_ENABLE_PIN_CS
  OLED_setGPIO_CS(0);
#  endif
#  ifdef OLED_ENABLE_PIN_DC
  OLED_setGPIO_DC(1);
#  endif

  OLED_StatusTypeDef status;
#  ifdef OLED_USING_DMA_TRANSMIT
  gb_call_refresh = 1;
  if (0 == g_call_refresh_count % 2) {
    uint8_t oled_write_param[CALC_NUM_LENGTH(__oled_write_param)];
    for (int i = 0; i < CALC_NUM_LENGTH(__oled_write_param); ++i) oled_write_param[i] = __oled_write_param[i];
    oled_write_param[0] += g_call_refresh_count / 2;
    OLED_WriteCmd(oled_write_param, CALC_NUM_LENGTH(oled_write_param));
  } else {
    status =
      OLED_Transmit(OLED_PHY_ADDRESS, 0x40, (uint8_t *)g_oled_buffer[(g_call_refresh_count - 1) / 2], OLED_PIX_WIDTH);
  }
#  else
  /**
   * @note 按页写
   * @note 为了方便后期写参数，这里对原来的参数数组进行了拷贝
   */
  uint8_t oled_write_param[CALC_NUM_LENGTH(__oled_write_param)];
  for (int i = 0; i < CALC_NUM_LENGTH(__oled_write_param); ++i) oled_write_param[i] = __oled_write_param[i];

  for (int i = 0; i < OLED_PAGE_SIZE; ++i) {
    OLED_WriteCmd(oled_write_param, CALC_NUM_LENGTH(oled_write_param));
    uint8_t *ptemp = g_oled_buffer[i];
    status = OLED_Transmit(OLED_PHY_ADDRESS, 0x40, (uint8_t *)ptemp, OLED_PIX_WIDTH);
    oled_write_param[0]++;
  }
#  endif

#  ifdef OLED_ENABLE_PIN_CS
#    ifndef OLED_USING_DMA_TRANSMIT
  OLED_setGPIO_CS(1);
#    endif
#  endif
  return status;
}
#  ifdef OLED_USING_DMA_TRANSMIT
/**
 * 为了尽可能的降低对CPU的占用
 * 这里使用DMA传输完成回调，请把下面这个函数放到DMA完成回调里面调用
 */
void OLED_DMA_TxCpltback()
{
  if (1 == gb_call_refresh && g_call_refresh_count / 2 < OLED_PAGE_SIZE) {
    OLED_Refresh_GSRAM();
    g_call_refresh_count++;
  } else {
    g_call_refresh_count = 0;
    gb_call_refresh = 0;
  }
}
#  endif
#endif

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
#ifdef OLED_ENABLE_PIN_CS
  OLED_setGPIO_CS(0);
#endif
#ifdef OLED_ENABLE_PIN_DC
  OLED_setGPIO_DC(0);
#endif
  /**
   * @note 拷贝一份原有数据防止函数退出后，pcmd指向的地址失效
   * @note 进而导致DMA发送数据异常
   */
  for (int i = 0; i < total; ++i) g_command_buffer[i] = pcmd[i];
  OLED_StatusTypeDef status = OLED_Transmit(OLED_PHY_ADDRESS, 0x00, g_command_buffer, total);
#ifdef OLED_NO_WAIT_TRANSMIT_PROCESS
  OLED_DelayMS(1);
#endif
#ifndef OLED_ENABLE_PIN_CS
  OLED_setGPIO_CS(1);
#endif
  return status;
}
