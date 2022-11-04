# OLED Driver

> 本仓库用于提供一个OLED屏幕的通用驱动框架库

## Support IC

> 目前支持的IC列表，如果你有需要也可以提`ISSUE`

* SSD1306

# 使用说明

## IIC

> 这里采用`STM32`下的`HAL`库作为说明，这里默认你已经正确配置并连接了相关总线

1. 在`main.h`引入头文件
2. 在`main.c`的`USER CODE 4 BEGIN`这里添加以下代码段，其实就是定义了传输函数以及延时函数，宏定义是关于是否等待的开关

```C
OLED_StatusTypeDef OLED_I2C_Transmit(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef I2C_State;
#ifndef OLED_NO_WAIT_TRANSMIS_PROCESS
  do {
#endif
    I2C_State = HAL_I2C_Mem_Write_DMA(&hi2c1, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size);
#ifndef OLED_NO_WAIT_TRANSMIS_PROCESS
  } while (I2C_State == HAL_BUSY);
#endif
  return OLED_OK;
}
#ifdef OLED_NO_WAIT_TRANSMIS_PROCESS
void OLED_DelayMS(uint8_t ms) { HAL_Delay(ms); }
#endif
```

3. 在主循环前初始化OLED，函数名为`OLED_Init()`

## 编译器相关

### Keil

按照百度的方法把本仓库的源码添加到项目里面

### CMakeLists.txt

1. `add_subdirectory(path/OLEDDriver)`
2. 在`add_executable`后面追加`target_link_libraries(${PROJECT_NAME}.elf PRIVATE OLEDDrive)`