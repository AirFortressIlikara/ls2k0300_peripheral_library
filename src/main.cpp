/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-11-30 09:06:41
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2024-12-12 06:21:45
 * @FilePath: /ls2k0300_peripheral_library/src/main.cpp
 * @Description: 测试用主程序
 *
 * Copyright (c) 2024 by ilikara 3435193369@qq.com, All Rights Reserved.
 */
#include "pwm_gtim.h"
#include "pwm_atim.h"
#include "encoder.h"
#include <iostream>
int main()
{
    // ENCODER encoder(0, 67);
    int gpio, ch, per, duty;
    // std::cin >> gpio >> ch >> per >> duty;
    // PWM_ATIM test(gpio, 0b11, ch, per, duty);
    PWM_ATIM test(86, 0b11, 3, 200000, 50000, 1);
    test.enable();
    for (int i = 0; i < 100; ++i)
    {
        // std::cout << encoder.pulse_counter_update() << std::endl;
        // usleep(5000);
        std::cin >> per >> duty;
        test.setPeriod(per);
        test.setDutyCycle(duty);
    }
    return 0;
}