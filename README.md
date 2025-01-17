<!--
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-11-30 12:24:30
 * @LastEditors: Ilikara 3435193369@qq.com
 * @LastEditTime: 2025-01-17 19:16:39
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

## 久久派点灯
久久派上其中一个LED与CPU直连，可以点灯（雾）

| 引脚号 | GPIO复用 | 主功能复用 | 第一复用  |  第二复用  |
| :----: | :------: | :--------: | :-------: | :--------: |
| S_LED  |  GPIO83  |  tim1_ch3  | spi3_miso | i2c_sda[2] |

## 久久派引脚功能复用表
久久派引出io的复用功能见下表。

在pwm_gtim中有mux参数，GPIO复用为0b00，第一复用为0b01，第二复用为0b10，主功能复用为0b11。若需要在基于2K0300的其他产品中使用该库，请查询数据手册的完整复用表。

| 引脚号 | GPIO复用 | 主功能复用 |  第一复用   |       第二复用       | 引脚号 | GPIO复用 | 主功能复用 |    第一复用    |       第二复用       |
| :----: | :------: | :--------: | :---------: | :------------------: | :----: | :------: | :--------: | :------------: | :------------------: |
|   1    |    -     |    GND     |      -      |          -           |   2    |    -     |    P3V3    |       -        |          -           |
|   3    |  GPIO88  |  tim2_ch2  | sdio1_d[6]  |        pwm[2]        |   4    |  GPIO41  |  uart0_tx  | gmac0_ptp_pps  |     lio_data[1]      |
|   5    |  GPIO89  |  tim2_ch3  | sdio1_d[7]  |        pwm[3]        |   6    |  GPIO40  |  uart0_rx  | gmac0_ptp_trig |     lio_data[0]      |
|   7    |  GPIO73  | can_tx[2]  | sdio1_d[5]  |      gmac0_crs       |   8    |  GPIO43  |  uart1_tx  | gmac1_ptp_pps  |     lio_data[3]      |
|   9    |  GPIO72  | can_rx[2]  | sdio1_d[4]  |      gmac0_col       |   10   |  GPIO42  |  uart1_rx  | gmac1_ptp_trig |     lio_data[2]      |
|   11   |  GPIO75  | can_tx[3]  | sdio1_d[7]  |      gmac1_crs       |   12   |  GPIO45  |  uart2_rx  |  gmac1_rx[0]   |     lio_data[5]      |
|   13   |  GPIO74  | can_rx[3]  | sdio1_d[6]  |      gmac1_col       |   14   |  GPIO44  |  uart2_tx  |  gmac1_rx_ctl  |     lio_data[4]      |
|   15   |  GPIO50  | i2c_scl[1] | gmac1_tx[0] |     lio_data[10]     |   16   |  GPIO48  | i2c_scl[0] |  gmac1_rx[3]   |     lio_data[8]      |
|   17   |  GPIO51  | i2c_sda[1] | gmac1_tx[1] |     lio_data[11]     |   18   |  GPIO49  | i2c_sda[0] |  gmac1_tx_ctl  |     lio_data[9]      |
|   19   |    -     |    GND     |      -      |          -           |   20   |    -     |    GND     |       -        |          -           |
|   21   |  GPIO64  |  spi2_clk  |   pwm[0]    | uart0_dcd / uart5_rx |   22   |  GPIO60  |  spi1_clk  |   i2c_scl[0]   | uart0_rts / uart6_tx |
|   23   |  GPIO67  |  spi2_cs   |   pwm[3]    | uart1_cts / uart9_rx |   24   |  GPIO63  |  spi1_cs   |   i2c_sda[1]   | uart0_dtr / uart4_tx |
|   25   |  GPIO65  | spi2_miso  |   pwm[1]    | uart0_ri / uart5_tx  |   26   |  GPIO61  | spi1_miso  |   i2c_sda[0]   | uart0_cts / uart6_rx |
|   27   |  GPIO66  | spi2_mosi  |   pwm[2]    | uart1_rts / uart9_tx |   28   |  GPIO62  | spi1_mosi  |   i2c_scl[1]   | uart0_dsr / uart4_rx |
|   29   |    -     |    GND     |      -      |          -           |   30   |    -     |    P5V     |       -        |          -           |

**注意**
- 16、18号引脚与LCD屏幕的触控共用，不建议使用
- 4、6号引脚对应UART0，为默认调试引脚，不建议使用
- 6、10、14号引脚只能输出，不能输入

## 久久派LCD引脚功能复用表
久久派54Pin FPC引出io的复用功能见下表。

在pwm_atim中有mux参数，GPIO复用为0b00，第一复用为0b01，第二复用为0b10，主功能复用为0b11。若需要在基于2K0300的其他产品中使用该库，请查询数据手册的完整复用表。

| 引脚号 | GPIO复用 |  主功能复用  |   第一复用   |   第二复用   | 引脚号 | GPIO复用 |  主功能复用  |  第一复用   |   第二复用   |
| :----: | :------: | :----------: | :----------: | :----------: | :----: | :------: | :----------: | :---------: | :----------: |
|   1    |    -     |      -       |      -       |      -       |   2    |    -     |      -       |      -      |      -       |
|   3    |    -     |      -       |      -       |      -       |   4    |    -     |      -       |      -      |      -       |
|   5    |    -     |     GND      |      -       |      -       |   6    |  GPIO54  |  i2c_scl[3]  | gmac1_mdck  | lio_data[14] |
|   7    |  GPIO49  |  i2c_sda[0]  | gmac1_tx_ctl | lio_data[9]  |   8    |  GPIO48  |  i2c_scl[0]  | gmac1_rx[3] | lio_data[8]  |
|   9    |    -     |      -       |      -       |      -       |   10   |    -     |      -       |      -      |      -       |
|   11   |    -     |      -       |      -       |      -       |   12   |    -     |      -       |      -      |      -       |
|   13   |  GPIO52  |  i2c_scl[2]  | gmac1_tx[2]  | lio_data[12] |   14   |  GPIO53  |  i2c_sda[2]  | gmac1_tx[3] | lio_data[13] |
|   15   |    -     |     GND      |      -       |      -       |   16   |  GPIO04  | lcd_dat_b[0] |      -      |   lioa[3]    |
|   17   |  GPIO05  | lcd_dat_b[1] |      -       |   lioa[4]    |   18   |  GPIO06  | lcd_dat_b[2] |      -      |   lioa[5]    |
|   19   |  GPIO07  | lcd_dat_b[3] |      -       |   lioa[6]    |   20   |  GPIO08  | lcd_dat_b[4] |      -      |   lioa[7]    |
|   21   |  GPIO09  | lcd_dat_b[5] |      -       |   lioa[8]    |   22   |  GPIO10  | lcd_dat_b[6] |      -      |   lioa[9]    |
|   23   |  GPIO11  | lcd_dat_b[7] |      -       |   lioa[10]   |   24   |    -     |     GND      |      -      |      -       |
|   25   |  GPIO12  | lcd_dat_g[0] |      -       |   lioa[11]   |   26   |  GPIO13  | lcd_dat_g[1] |      -      |   lioa[12]   |
|   27   |  GPIO14  | lcd_dat_g[2] |      -       |   lioa[13]   |   28   |  GPIO15  | lcd_dat_g[3] |      -      |   lioa[14]   |
|   29   |  GPIO16  | lcd_dat_g[4] |      -       |   lioa[15]   |   30   |  GPIO17  | lcd_dat_g[5] |      -      |   lioa[16]   |
|   31   |  GPIO18  | lcd_dat_g[6] |      -       |   lioa[17]   |   32   |  GPIO19  | lcd_dat_g[7] |      -      |   lioa[18]   |
|   33   |    -     |     GND      |      -       |      -       |   34   |  GPIO20  | lcd_dat_r[0] |      -      |   lioa[19]   |
|   35   |  GPIO21  | lcd_dat_r[1] |      -       |   lioa[20]   |   36   |  GPIO22  | lcd_dat_r[2] |      -      |   lioa[21]   |
|   37   |  GPIO23  | lcd_dat_r[3] |      -       |   lioa[22]   |   38   |  GPIO24  | lcd_dat_r[4] |      -      |  liocsn[0]   |
|   39   |  GPIO25  | lcd_dat_r[5] |      -       |  liocsn[1]   |   40   |  GPIO26  | lcd_dat_r[6] |      -      |    liowrn    |
|   41   |  GPIO27  | lcd_dat_r[7] |      -       |    liordn    |   42   |    -     |     GND      |      -      |      -       |
|   43   |  GPIO02  |  lcd_hsync   |      -       |   lioa[1]    |   44   |  GPIO01  |  lcd_vsync   |      -      |   lioa[0]    |
|   45   |  GPIO03  |    lcd_en    |      -       |   lioa[2]    |   46   |    -     |     GND      |      -      |      -       |
|   47   |  GPIO00  |   lcd_clk    |      -       |      -       |   48   |    -     |     GND      |      -      |      -       |
|   49   |  GPIO86  |  tim1_ch3n   |  sdio1_d[4]  |    pwm[0]    |   50   |    -     |     P5V      |      -      |      -       |
|   51   |    -     |     P5V      |      -       |      -       |   52   |    -     |     P5V      |      -      |      -       |
|   53   |    -     |     P5V      |      -       |      -       |   54   |    -     |     P5V      |      -      |      -       |

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

## 2K0300 p7zip 跑分

运行结果
```
7-Zip (a) 17.05 : Copyright (c) 1999-2021 Igor Pavlov : 2017-08-28
p7zip Version 17.05 (locale=en_US.utf8,Utf16=on,HugeFiles=on,64 bits,1 CPU LE)

LE
CPU Freq: 64000000 - - - - - - - -

RAM size:     378 MB,  # CPU hardware threads:   1
RAM usage:    219 MB,  # Benchmark threads:      1

                       Compressing  |                  Decompressing
Dict     Speed Usage    R/U Rating  |      Speed Usage    R/U Rating
         KiB/s     %   MIPS   MIPS  |      KiB/s     %   MIPS   MIPS

22:        537    96    545    523  |       9564    97    843    817
23:        519    97    546    529  |       9441    97    842    817
24:        504    97    559    543  |       9350    98    841    821
----------------------------------  | ------------------------------
Avr:              97    550    532  |               97    842    818
Tot:              97    696    675
```
