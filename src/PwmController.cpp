/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-09-17 08:21:50
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2025-04-12 09:23:23
 * @FilePath: /ls2k0300_peripheral_library/src/PwmController.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "PwmController.h"

#include <fstream>
#include <iostream>

PwmController::PwmController(int pwmchip, int pwmnum)
    : pwmchip(pwmchip)
    , pwmnum(pwmnum)
{
    pwmPath = "/sys/class/pwm/pwmchip" + std::to_string(pwmchip) + "/pwm" + std::to_string(pwmnum) + "/";
}

PwmController::~PwmController()
{
    disable(); // 析构时自动禁用PWM
}

int PwmController::readPeriod()
{
    return period;
}

int PwmController::readDutyCycle()
{
    return duty_cycle;
}

// 初始化PWM设备
bool PwmController::initialize()
{
    std::string exportPath = "/sys/class/pwm/pwmchip" + std::to_string(pwmchip) + "/export";
    return writeToFile(exportPath, std::to_string(pwmnum));
}

// 启用PWM
bool PwmController::enable()
{
    return writeToFile(pwmPath + "enable", std::to_string(1));
}

// 禁用PWM
bool PwmController::disable()
{
    return writeToFile(pwmPath + "enable", std::to_string(0));
}

// 设置周期（以纳秒为单位）
bool PwmController::setPeriod(unsigned int period_ns)
{
    period = period_ns;
    return writeToFile(pwmPath + "period", std::to_string(period_ns));
}

// 设置低电平时间（以纳秒为单位）
bool PwmController::setDutyCycle(unsigned int duty_cycle_ns)
{
    duty_cycle = duty_cycle_ns;
    return writeToFile(pwmPath + "duty_cycle", std::to_string(duty_cycle_ns));
}

// 向文件写入值的辅助函数
bool PwmController::writeToFile(const std::string& path, const std::string& value)
{
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << path << std::endl;
        return false;
    }
    file << value;
    file.close();
    return true;
}
