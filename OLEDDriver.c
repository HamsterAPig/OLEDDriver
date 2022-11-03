/**
 * @Description OLED驱动程序
 * @Author jinming xi
 * @Date 2022/11/3
 * @Project OLEDDriver
*/
#include "OLEDDriver.h"

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
OLED_StatusTypeDef OLED_Init(void) {
    const uint8_t _ssd1306_init_parm[] = {0xae,0x20,0x00,0xb0,0xc8,0x00,0x10,0x40,
                                          0x81,0xff,0xa1,0xa6,0xa8,0x3f,0xa4,
                                          0xd3,0x00,0xd5,0xf0,0xd9,0x22,0xda,
                                          0x12,0xdb,0x20,0x8d,0x14,0xaf};

}

/**
 * 将g_oled_buffer里面的内容更新到屏幕中
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_Reflush_GSRAM() {
    OLED_StatusTypeDef status;
    const uint8_t _ssd1306_write_command_parm[] = {0xb0,0x00,0x10};
    status = OLED_WriteCmd(_ssd1306_write_command_parm, 3);
    if (status != OLED_OK) return status;
    status = OLED_I2C_Transmit(OLED_PHY_ADDRESS, 0x40, g_oled_buffer, OLED_PIX_WIDTH * OLED_PAGE_SIZE);
    return status;
}

/**
 * 点亮某一个点或者点灭某一个点
 * @param x 横坐标
 * @param y 纵坐标
 * @param state 点的状态，点亮为1，点灭为0
 * @return OLED Status
 */
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define	CLEAR_BIT(x, bit)	(x &= ~(1 << bit))	/* 清零第bit位 */
OLED_StatusTypeDef OLED_SetPoint(uint8_t x, uint8_t y, uint8_t state) {
    OLED_StatusTypeDef status;

    // 检查输入参数.超出范围自动返回错误
    if (x >= OLED_PIX_WIDTH || y >= OLED_PIX_HEIGHT) return OLED_ERROR;

    uint8_t fill_data = y % 8;
    uint8_t old_data = g_oled_buffer[(y -fill_data) / 8][x];
    if (0 == state)
        g_oled_buffer[(y -fill_data) / 8][x] = CLEAR_BIT(old_data, fill_data);
    else
        SET_BIT(g_oled_buffer[(y -fill_data) / 8][x], state << fill_data);

    return OLED_OK;
}

/**
 * 将g_oled_buffer里面的内容更新到屏幕中
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_Fill(uint8_t state) {
    memset(g_oled_buffer, state, sizeof(uint8_t) * OLED_PIX_WIDTH * OLED_PAGE_SIZE);
    OLED_Reflush_GSRAM();
}

/**
 * OLED 写命令函数，作用于仅限于本文件
 * @param pcmd 命令数组指针
 * @param total 命令长度
 * @return OLED Status
 */
OLED_StatusTypeDef OLED_WriteCmd(uint8_t *pcmd, uint16_t total) {
#ifdef __ENABLE_DMA_OLED
    /**
    * @note 拷贝一份原有数据防止函数退出后，pcmd指向的地址失效
    * @note 进而导致DMA发送数据异常
    */
    for (int i = 0; i < total; ++i)
        g_command_buffer[i] = pcmd[i];
    OLED_StatusTypeDef status = OLED_I2C_Transmit(OLED_PHY_ADDRESS, 0x00, g_command_buffer, total);
#else
    //Todo 这里写不用DMA传输的过程
#endif
    return status;
}