/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-11-30 08:14:34
 * @LastEditors: Ilikara 3435193369@qq.com
 * @LastEditTime: 2025-02-14 09:54:52
 * @FilePath: /ls2k0300_peripheral_library/lib/pwm_gtim.h
 * @Description: 基于LS2K0300 GTIMER的PWM控制器类，可使用复用为TIM2_CHx的引脚
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
#ifndef PWM_GTIM_H_
#define PWM_GTIM_H_

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

#include "register.h"

#define GPIO_MUX_BASE_ADDR 0x16000490

#define GTIM_BASE_ADDR 0x16119000
#define GTIM_CR1_OFFSET 0x00
#define GTIM_CR2_OFFSET 0x04
#define GTIM_SMCR_OFFSET 0x08
#define GTIM_DIER_OFFSET 0x0C
#define GTIM_SR_OFFSET 0x10
#define GTIM_EGR_OFFSET 0x14
#define GTIM_CCMR1_OFFSET 0x18
#define GTIM_CCMR2_OFFSET 0x1C
#define GTIM_CCER_OFFSET 0x20
#define GTIM_CNT_OFFSET 0x24
#define GTIM_PSC_OFFSET 0x28
#define GTIM_ARR_OFFSET 0x2C
#define GTIM_CCR1_OFFSET 0x34
#define GTIM_CCR2_OFFSET 0x38
#define GTIM_CCR3_OFFSET 0x3C
#define GTIM_CCR4_OFFSET 0x40
#define GTIM_INSTA_OFFSET 0x50

class PWM_GTIM
{
public:
    PWM_GTIM(int gpio, int mux, int chNum_, int period_, int duty_cycle_);
    ~PWM_GTIM(void);

    void enable(void);
    void disable(void);
    void setPeriod(unsigned int period_10ns_);
    void setDutyCycle(unsigned int duty_cycle_10ns_);
    uint32_t period_10ns, duty_cycle_10ns;

private:
    uint32_t chNum;
    void *ccmr_buffer[2];
    void *ccer_buffer;
    void *period_buffer;
    void *duty_cycle_buffer;
    void *cnt_buffer;
};

#endif