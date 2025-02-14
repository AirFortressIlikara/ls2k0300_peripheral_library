/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-10-11 06:20:04
 * @LastEditors: Ilikara 3435193369@qq.com
 * @LastEditTime: 2025-02-14 09:55:03
 * @FilePath: /ls2k0300_peripheral_library/lib/encoder.h
 * @Description: 基于PWM控制器实现的方向编码器类
 *
 * Copyright (c) 2024 by ilikara 3435193369@qq.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "GPIO.h"
#include "register.h"

#define ENCODER_PPR 1024 // 编码器线数

#define PWM_BASE_ADDR 0x1611B000
#define PWM_OFFSET 0x10
#define LOW_BUFFER_OFFSET 0x4
#define FULL_BUFFER_OFFSET 0x8
#define CONTROL_REG_OFFSET 0xC

#define CNTR_ENABLE_BIT (1 << 0)       // 计数器使能
#define PULSE_OUT_ENABLE_BIT (1 << 3)  // 脉冲输出使能（低有效）
#define SINGLE_PULSE_BIT (1 << 4)      // 单脉冲控制位
#define INT_ENABLE_BIT (1 << 5)        // 中断使能
#define INT_STATUS_BIT (1 << 6)        // 中断状态
#define COUNTER_RESET_BIT (1 << 7)     // 计数器重置
#define MEASURE_PULSE_BIT (1 << 8)     // 测量脉冲使能
#define INVERT_OUTPUT_BIT (1 << 9)     // 输出翻转使能
#define DEAD_ZONE_ENABLE_BIT (1 << 10) // 防死区使能

class ENCODER
{
public:
    ENCODER(int pwmNum, int gpioNum);
    ~ENCODER(void);

    double pulse_counter_update(void);

private:
    uint32_t base_addr;
    GPIO directionGPIO;
    void *low_buffer;
    void *full_buffer;
    void *control_buffer;
    void PWM_Init(void);
    void reset_counter(void);
};

#endif
