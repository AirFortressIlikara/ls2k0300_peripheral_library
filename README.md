<!--
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-11-30 12:24:30
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2025-01-05 15:41:45
 * @FilePath: /ls2k0300_peripheral_library/README.md
 * @Description: github README
 * 
 * Copyright (c) 2024 by ilikara 3435193369@qq.com, All Rights Reserved. 
-->
# ls2k0300_peripheral_library

欢迎关注[我的bilibili个人主页](https://space.bilibili.com/317252912)
- [交叉编译FFmpeg](https://www.bilibili.com/opus/1006195805380411430)
- [交叉编译OpenCV](https://www.bilibili.com/video/BV1Q2zbYrEvS/)
- [（已过时）GTIM驱动编译与安装](https://www.bilibili.com/video/BV1Ddi2YmESo/)
- [在设备树中加入SPI1的屏幕节点](https://bbs.ctcisz.com/forum.php?mod=viewthread&tid=142)
- [救砖教程：局域网tftp加载内核开机](https://www.bilibili.com/video/BV1hvqJY3EWu/)
- 久久派拓展板毛坯房 在OSHwhub未过审，请去qq群自行下载
- [久久派旧世界Linux 4.19源码](https://github.com/AirFortressIlikara/LS2K0300-linux-4.19) 该部分已经集成了我的ATIM GTIM驱动，并提供了久久派的智能车defconfig预设

后续会有更多视频教程发布。

## 这个仓库提供了什么

测试过的功能
- 基于ATIM控制器的PWM类
- 基于ATIM控制器的PWM驱动
- 基于GTIM控制器的PWM类
- 基于GTIM控制器的PWM驱动
- 基于PWM控制器的方向编码器类

未测试的功能
- 暂无

开发中的功能
- 总钻风驱动程序

开发计划
- 提供更多TIM驱动程序
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
- 16、18号引脚与LCD屏幕的触控共用，不建议使用
- 4、6号引脚对应UART0，为默认调试引脚，不建议使用
- 6、10、14号引脚只能输出，不能输入

## 久久派LCD引脚功能复用表
久久派54Pin FPC引出io的常用复用功能见下表，不常用功能未列举，请自行查询手册。

在pwm_atim中有mux参数，GPIO复用为0b00，第一复用为0b01，第二复用为0b10，主功能复用为0b11。若需要在基于2K0300的其他产品中使用该库，请查询数据手册的完整复用表。

|引脚号|GPIO复用|主功能复用|第一复用|第二复用|引脚号|GPIO复用|主功能复用|第一复用|第二复用|
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|1|NC||||2|NC||||
|3|NC||||4|NC||||
|5|GND||||6|GPIO54|I2C_SCL3|||
|7|GPIO49|I2C_SDA0|||8|GPIO48|I2C_SCL0|||
|9|NC||||10|NC||||
|11|NC||||12|NC||||
|13|GPIO52|I2C_SCL2|||14|GPIO53|I2C_SDA2|||
|15|GND||||16|GPIO04|lcd_dat_b[0]|||
|17|GPIO05|lcd_dat_b[1]|||18|GPIO06|lcd_dat_b[2]|||
|19|GPIO07|lcd_dat_b[3]|||20|GPIO08|lcd_dat_b[4]|||
|21|GPIO09|lcd_dat_b[5]|||22|GPIO10|lcd_dat_b[6]|||
|23|GPIO11|lcd_dat_b[7]|||24|GND||||
|25|GPIO12|lcd_dat_g[0]|||26|GPIO13|lcd_dat_g[1]|||
|27|GPIO14|lcd_dat_g[2]|||28|GPIO15|lcd_dat_g[3]|||
|29|GPIO16|lcd_dat_g[4]|||30|GPIO17|lcd_dat_g[5]|||
|31|GPIO18|lcd_dat_g[6]|||32|GPIO19|lcd_dat_g[7]|||
|33|GND||||34|GPIO20|lcd_dat_r[0]|||
|35|GPIO21|lcd_dat_r[1]|||36|GPIO22|lcd_dat_r[2]|||
|37|GPIO23|lcd_dat_r[3]|||38|GPIO24|lcd_dat_r[4]|||
|39|GPIO25|lcd_dat_r[5]|||40|GPIO26|lcd_dat_r[6]|||
|41|GPIO27|lcd_dat_r[7]|||42|GND||||
|43|GPIO02|lcd_hsync|||44|GPIO01|lcd_vsync|||
|45|GPIO03|lcd_en|||46|GND||||
|47|GPIO00|lcd_clk|||48|GND||||
|49|GPIO86|TIM1_CH3N||PWM0|50|P5V||||
|51|P5V||||52|P5V||||
|53|P5V||||54|P5V||||

**注意**
- 7、8号触控引脚在排针处也被被引出
- 16-22号引脚为芯片初始化信号，不建议使用

## 设备树部分修改教程

请勿继续使用0615，0627等过老的linux源码。

**make menuconfig** 时 **Kernal Type->Built-In DTB** 请根据自己板卡的型号填写，例如久久派wifi版应选择 **loongson_2k0300_pai_99_wifi** 。

这里假设你使用的是久久派wifi版，下面的路径为在linux源码目录下的相对路径
>arch/loongarch/boot/dts/loongson/loongson_2k0300_pai_99_wifi.dts

- 修改形如 **pinctrl-0 = <&i2c0_mux_m0>;** 的部分以配置GPIO复用配置
- 修改形如 **status = "okay";** 的部分以使能某功能

>arch/loongarch/boot/dts/loongson/2k0300-pinctrl.dtsi

- 查询复用配置对应的引脚号和功能号，例如 **<&gpa4 2 6>** 代表了4\*16+2到4\*16+6即GPIO66-70

## 2K0300 CoreMark 跑分

使用 **git clone** 下载最新源码。

编译命令：
```
make CC=loongarch64-linux-gnu-gcc link
```
运行结果：
```
2K performance run parameters for coremark.
CoreMark Size    : 666
Total ticks      : 16488
Total time (secs): 16.488000
Iterations/Sec   : 3639.010189
Iterations       : 60000
Compiler version : GCC8.3.0
Compiler flags   : -O2   -lrt
Memory location  : Please put data memory location here
			(e.g. code in flash, data on heap etc)
seedcrc          : 0xe9f5
[0]crclist       : 0xe714
[0]crcmatrix     : 0x1fd7
[0]crcstate      : 0x8e3a
[0]crcfinal      : 0xbd59
Correct operation validated. See README.md for run and reporting rules.
CoreMark 1.0 : 3639.010189 / GCC8.3.0 -O2   -lrt / Heap
```
