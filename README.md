# OLED Driver

> 本仓库用于提供一个OLED屏幕的通用驱动框架库，考虑了许久还是决定把关键传输函数的实现细节暴露给用户，由用户实现。

## Support IC

> 目前支持的IC列表，如果你有需要也可以提`ISSUE`

* SSD1306
* SH1106

# 使用说明

## IIC

> 这里采用`STM32`下的`HAL`库作为说明，这里默认你已经正确配置并连接了相关总线

### 不启用DMA

1. 包含本项目头文件`OLEDDriver.h`
2. 在外设初始化函数调用的后面加上`OLED_Init()`以便初始化`OLED`
3. 项目定义全局宏变量，其作用位置在`OLEDDriverParam.h`文件中，相关宏如下
   * `__USING_SSD1306`: 使用SH1106驱动相关参数
4. 用户实现一些相关的前后调用函数，如下

```c
void OLED_WriteCmd_CallAfter() { HAL_Delay(1); }
void OLED_Refresh_GSRAM_CallAfter() { HAL_Delay(1); }
```

5. 用户实现I2C传输函数，如下

```c
OLED_StatusTypeDef OLED_Transmit(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint8_t mode)
{
  HAL_I2C_Mem_Write(&hi2c1, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, 0xff);
}
```



至此，I2C普通驱动移植完毕

## SPI

### 不启用DMA

1. 包含本项目头文件`OLEDDriver.h`
2. 项目定义全局宏变量，其作用位置在`OLEDDriverParam.h`文件中，相关宏如下
   * `__USING_SH1106`: 使用SH1106驱动相关参数
   * `OLED_USING_PAGE_MODE`: 使用页地址模式

3. 在外设初始化函数调用的后面加上`OLED_Init()`以便初始化`OLED`

> 注意：为了保证设备总线能够及时把初始化数据发送出去，这里必须予以一定的延时，一般给100ms

4. 用户自己实现一些前调函数，相关函数如下

```C
void OLED_WriteCmd_CallBefore()
{
  OLED_setGPIO_DC(0);
  OLED_setGPIO_CS(0);
}
void OLED_Refresh_GSRAM_CallBefore()
{
  OLED_setGPIO_DC(1);
  OLED_setGPIO_CS(0);
}
```

4. 用户实现传输函数，这里使用`STM32 HAL`作为示范

```c
OLED_StatusTypeDef OLED_Transmit(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint8_t mode)
{
  if (0 == mode) {
    OLED_WriteCmd_CallBefore();
    HAL_SPI_Transmit(&hspi1, pData, Size, 0xff);
  } else {
    uint8_t __write_cmd[] = {0xb0, 0x02, 0x10};
    for (int i = 0; i < OLED_PAGE_SIZE; ++i) {
      OLED_WriteCmd_CallBefore();
      HAL_SPI_Transmit(&hspi1, __write_cmd, 3, 0xff);
      OLED_Refresh_GSRAM_CallBefore();
      HAL_SPI_Transmit(&hspi1, (pData + OLED_PIX_WIDTH * i), OLED_PIX_WIDTH, 0xff);
      __write_cmd[0]++;
    }
  }
}
```

至此，SPI不适用DMA的驱动移植完毕

## 启用DMA传输

> 考虑到使用的`SH1106`芯片无法设置水平寻址模式，页寻址需要多次发送，DMA传输的时候如果使用while等待总线空闲相当于没有使用DMA，因为CPU一直在等待。这里采用DMA传输完成回调函数，使用两个全局状态变量

具体步骤参考`SPI 普通传输`的1~3步，第四步需要修改的代码为

```c
uint8_t g_oled_transmit_count = 0;       // 回调函数计数
uint8_t gb_oled_transmit_data_mode = 0;  // 确定是由于传输数据过程中触发的传输完成中断
OLED_StatusTypeDef OLED_Transmit(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint8_t mode)
{
  if (0 == mode) {
    OLED_WriteCmd_CallBefore();
    HAL_SPI_Transmit_DMA(&hspi1, pData, Size);
  } else {
    gb_oled_transmit_data_mode = 1;
    if (0 == g_oled_transmit_count % 2) {
      uint8_t __write_cmd[] = {0xb0, 0x02, 0x10};
      OLED_WriteCmd_CallBefore();
      __write_cmd[0] += g_oled_transmit_count / 2;
      HAL_SPI_Transmit_DMA(&hspi1,__write_cmd, 3);
    } else {
      OLED_Refresh_GSRAM_CallBefore();
      HAL_SPI_Transmit_DMA(&hspi1, (pData + OLED_PIX_WIDTH * ((g_oled_transmit_count - 1) / 2)), OLED_PIX_WIDTH);
    }
  }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi == &hspi1 && 1 == gb_oled_transmit_data_mode) {
    g_oled_transmit_count++;
    if (g_oled_transmit_count / 2 >= OLED_PAGE_SIZE) {
      gb_oled_transmit_data_mode = 0;
      g_oled_transmit_count = 0;
      return;
    }
    OLED_Transmit(0,0,(uint8_t *)g_oled_buffer, 0,1);
  }
}
```

## 其它

这里也可以考虑使用`RTOS`，使用事件标志位做任务状态，避免CPU资源的浪费

简单的说就是在每一次发送一页数据之后，挂起任务

进入中断之后重新唤醒发送任务

## 编译器相关

### Keil

按照百度的方法把本仓库的源码添加到项目里面

### CMakeLists.txt

1. `add_subdirectory(path/OLEDDriver)`
2. 在`add_executable`后面追加`target_link_libraries(${PROJECT_NAME}.elf PRIVATE OLEDDrive)`