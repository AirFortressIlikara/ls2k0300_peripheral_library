/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-09-17 08:12:04
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2025-04-12 09:23:18
 * @FilePath: /ls2k0300_peripheral_library/lib/PwmController.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H

#include <string>

class PwmController {
public:
    PwmController(int pwmchip, int pwmnum);
    ~PwmController();

    bool enable();
    bool disable();
    bool setPeriod(unsigned int period_ns);
    bool setDutyCycle(unsigned int duty_cycle_ns);
    bool initialize();
    int readPeriod();
    int readDutyCycle();

private:
    std::string pwmPath; // PWM设备的路径
    int pwmchip;
    int pwmnum;
    int period;
    int duty_cycle;
    bool writeToFile(const std::string& path, const std::string& value);
};

#endif
