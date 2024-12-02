<!--
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-11-30 12:24:30
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2024-12-02 09:32:44
 * @FilePath: /ls2k0300_peripheral_library/README.md
 * @Description: github README
 * 
 * Copyright (c) 2024 by ilikara 3435193369@qq.com, All Rights Reserved. 
-->
# ls2k0300_peripheral_library

欢迎关注我的bilibili个人主页：https://space.bilibili.com/317252912
- 交叉编译FFmpeg https://www.bilibili.com/opus/1006195805380411430
- 交叉编译OpenCV https://www.bilibili.com/video/BV1Q2zbYrEvS/
- 在设备树中加入SPI1的屏幕节点 https://bbs.ctcisz.com/forum.php?mod=viewthread&tid=142

后续会有更多视频教程发布。

## 这个仓库提供了什么

测试过的功能
- 基于GTIM控制器的PWM类
- 基于PWM控制器的方向编码器

未测试的功能
- 基于ATIM控制器的PWM类

开发计划
- 修改为驱动程序
- 迁移到RUST

本仓库持续更新中

## 久久派引脚功能复用表
久久派引出io的常用复用功能见下表，不常用功能未列举，请自行查询手册。

在pwm_gtim中有mux参数，GPIO复用为0b00，第一复用为0b01，第二复用为0b10，主功能复用为0b11。若需要在基于2K0300的其他产品中使用该库，请查询数据手册的完整复用表。

|引脚号|GPIO复用|主功能复用|第一复用|第二复用|引脚号|GPIO复用|主功能复用|第一复用|第二复用|
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|1|GND||||2|P3V3||||
|3|GPIO88|TIM2_CH2||PWM2|4|GPIO41|UART0_TX|||
|5|GPIO89|TIM2_CH3||PWM3|6|GPIO40|UART0_RX|||
|7|GPIO73|CAN_TX2|||8|GPIO43|UART1_TX|||
|9|GPIO72|CAN_RX2|||10|GPIO42|UART1_RX|||
|11|GPIO75|CAN_TX3|||12|GPIO45|UART2_TX|||
|13|GPIO74|CAN_RX3|||14|GPIO44|UART2_RX|||
|15|GPIO50|I2C_SCL1|||16|GPIO48|I2C_SCL0|||
|17|GPIO51|I2C_SDA1|||18|GPIO49|I2C_SDA0|||
|19|GND||||20|GND||||
|21|GPIO64|SPI2_CLK|PWM0|UART0_DCD|22|GPIO60|SPI1_CLK|I2C_SCL0|UART0_RTS|
|23|GPIO67|SPI2_CS|PWM3|UART1_CTS|24|GPIO63|SPI1_CS|I2C_SDA1|UART0_CTR|
|25|GPIO65|SPI2_MISO|PWM1|UART0_RI|26|GPIO61|SPI1_MISO|I2C_SDA0|UART0_CTS|
|27|GPIO66|SPI2_MOSI|PWM2|UART1_RTS|28|GPIO62|SPI1_MOSI|I2C_SCL1|UART0_DSR|
|29|GND||||30|P5V||||

**注意**
- 16、18号引脚与LCD屏幕的触控共用，不建议使用；
- 4、6号引脚对应UART0，为默认调试引脚，不建议使用；
- 6、10、14号引脚只能输入，不能输出
