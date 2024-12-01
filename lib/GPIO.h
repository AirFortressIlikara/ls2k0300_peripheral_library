/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-10-10 15:02:00
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2024-12-01 13:27:38
 * @FilePath: /ls2k0300_peripheral_library/lib/GPIO.h
 * @Description: GPIO类
 *
 * Copyright (c) 2024 by ilikara 3435193369@qq.com, All Rights Reserved.
 */

#ifndef GPIO_H
#define GPIO_H

#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

class GPIO
{
public:
    GPIO(int gpioNum_);
    ~GPIO(void);

    int getFileDescriptor(void) const;               // 获取 GPIO 文件描述符
    bool setDirection(const std::string &direction); // 设置GPIO方向，out为输出，in为输入
    bool setValue(bool value);                       // 设置 GPIO 输出值
    bool readValue(void);                            // 读取 GPIO 输入值
    bool setEdge(const std::string &edge);

private:
    int gpioNum;
    int fd;
    std::string gpioPath;

    bool writeToFile(const std::string &path, const std::string &value);
};

#endif // GPIO_H
