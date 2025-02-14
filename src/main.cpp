/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-11-30 09:06:41
 * @LastEditors: Ilikara 3435193369@qq.com
 * @LastEditTime: 2025-02-14 09:54:23
 * @FilePath: /ls2k0300_peripheral_library/src/main.cpp
 * @Description: 测试用主程序
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