<!--
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-11-30 12:24:30
 * @LastEditors: Ilikara 3435193369@qq.com
 * @LastEditTime: 2025-01-18 19:53:03
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
  - 已在[久久派旧世界Linux 4.19源码](https://github.com/AirFortressIlikara/LS2K0300-linux-4.19)预装
- [（已过时）在设备树中加入SPI1的屏幕节点](https://bbs.ctcisz.com/forum.php?mod=viewthread&tid=142)
  - 已在[久久派旧世界Linux 4.19源码](https://github.com/AirFortressIlikara/LS2K0300-linux-4.19)预装
- [救砖教程：局域网tftp加载内核开机](https://www.bilibili.com/video/BV1hvqJY3EWu/)
- 久久派拓展板毛坯房 在OSHwhub未过审，请去qq群自行下载
- [久久派旧世界Linux 4.19源码](https://github.com/AirFortressIlikara/LS2K0300-linux-4.19) 该部分已经集成了我的ATIM GTIM驱动，并提供了久久派的智能车defconfig预设
    ```
    git clone https://github.com/AirFortressIlikara/LS2K0300-linux-4.19
    cd LS2K0300-linux-4.19
    cp ./arch/loongarch/configs/loongson_2k0300_99pi_wifi_defconfig .config
    vim cmd.sh #这一步要修改cmd.sh
    ./cmd.sh
    ```

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

## 引脚功能复用表
- [2K0300功能复用表](https://github.com/AirFortressIlikara/ls2k0300_peripheral_library/blob/main/2K0300%E5%BC%95%E8%84%9A%E5%8A%9F%E8%83%BD%E5%A4%8D%E7%94%A8%E8%A1%A8.md)
- [久久派](https://github.com/AirFortressIlikara/ls2k0300_peripheral_library/blob/main/%E4%B9%85%E4%B9%85%E6%B4%BE%E5%BC%95%E8%84%9A%E5%8A%9F%E8%83%BD%E5%A4%8D%E7%94%A8%E8%A1%A8.md)
- [蜂鸟派](https://github.com/AirFortressIlikara/ls2k0300_peripheral_library/blob/main/%E8%9C%82%E9%B8%9F%E6%B4%BE%E5%BC%95%E8%84%9A%E5%8A%9F%E8%83%BD%E5%A4%8D%E7%94%A8%E8%A1%A8.md)
- [先锋派](https://github.com/AirFortressIlikara/ls2k0300_peripheral_library/blob/main/%E5%85%88%E9%94%8B%E6%B4%BE%E5%BC%95%E8%84%9A%E5%8A%9F%E8%83%BD%E5%A4%8D%E7%94%A8%E8%A1%A8.md)
- [米联派](https://github.com/AirFortressIlikara/ls2k0300_peripheral_library/blob/main/%E7%B1%B3%E8%81%94%E6%B4%BE%E5%BC%95%E8%84%9A%E5%8A%9F%E8%83%BD%E5%A4%8D%E7%94%A8%E8%A1%A8.md)

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
| 24:        504    97    559    543 | 9350    98    841    821 |
| ---------------------------------- | ------------------------ |
| Avr:              97    550    532 | 97    842    818         |
Tot:              97    696    675
```
