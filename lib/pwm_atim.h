/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-11-30 04:25:47
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2024-12-01 13:24:34
 * @FilePath: /ls2k0300_peripheral_library/lib/pwm_atim.h
 * @Description: 基于LS2K0300 ATIMER的PWM控制器类，可使用复用为TIM1_CHx的引脚，未测试
 *
 * Copyright (c) 2024 by ilikara 3435193369@qq.com, All Rights Reserved.
 */
#ifndef PWM_ATIM_H_
#define PWM_ATIM_H_

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

#define ATIM_BASE_ADDR 0x16118000
#define ATIM_CR1_OFFSET 0x00
#define ATIM_CR2_OFFSET 0x04
#define ATIM_SMCR_OFFSET 0x08
#define ATIM_DIER_OFFSET 0x0C
#define ATIM_SR_OFFSET 0x10
#define ATIM_EGR_OFFSET 0x14
#define ATIM_CCMR1_OFFSET 0x18
#define ATIM_CCMR2_OFFSET 0x1C
#define ATIM_CCER_OFFSET 0x20
#define ATIM_CNT_OFFSET 0x24
#define ATIM_PSC_OFFSET 0x28
#define ATIM_ARR_OFFSET 0x2C
#define ATIM_RCR_OFFSET 0x30
#define ATIM_CCR1_OFFSET 0x34
#define ATIM_CCR2_OFFSET 0x38
#define ATIM_CCR3_OFFSET 0x3C
#define ATIM_CCR4_OFFSET 0x40
#define ATIM_BDTR_OFFSET 0x44
#define ATIM_INSTA_OFFSET 0x50

class PWM_ATIM
{
public:
    PWM_ATIM(int gpio, int mux, int chNum_, int period_, int duty_cycle_);
    ~PWM_ATIM(void);

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