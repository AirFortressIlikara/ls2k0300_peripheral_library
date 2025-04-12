/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-10-11 06:19:57
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2025-04-12 09:27:20
 * @FilePath: /ls2k0300_peripheral_library/src/encoder.cpp
 * @Description: 基于PWM控制器实现的方向编码器
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
#include "encoder.h"

#include <stdio.h>

#include "register.h"

// pwmNum为STEP引脚连接的PWM通道编号，gpioNum为DIR引脚所接的GPIO编号
ENCODER::ENCODER(int pwmNum, int gpioNum)
    : base_addr(PWM_BASE_ADDR + pwmNum * PWM_OFFSET)
    , directionGPIO(gpioNum)
{
    directionGPIO.setDirection("in");

    control_buffer = map_register(base_addr + CONTROL_REG_OFFSET, PAGE_SIZE);
    low_buffer = map_register(base_addr + LOW_BUFFER_OFFSET, PAGE_SIZE);
    full_buffer = map_register(base_addr + FULL_BUFFER_OFFSET, PAGE_SIZE);

    printf("Registers mapped successfully\n");

    PWM_Init();
}

// 析构函数
ENCODER::~ENCODER(void)
{
    // directionGPIO.~GPIO();
    munmap(control_buffer, PAGE_SIZE);
    munmap(low_buffer, PAGE_SIZE);
    munmap(full_buffer, PAGE_SIZE);
}

// 初始化PWM控制器为计数模式
void ENCODER::PWM_Init(void)
{
    uint32_t control_reg = 0;

    control_reg |= CNTR_ENABLE_BIT;
    control_reg |= MEASURE_PULSE_BIT;
    control_reg |= INT_ENABLE_BIT;

    REG_WRITE(control_buffer, control_reg);

    printf("PWM initialized with control register: 0x%08X\n", control_reg);
}

// 返回编码器的RPS
double ENCODER::pulse_counter_update(void)
{
    double value = 100000000.0 / REG_READ(full_buffer) / ENCODER_PPR * (directionGPIO.readValue() * 2 - 1);
    return value;
}
