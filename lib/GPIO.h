/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-10-10 15:02:00
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2025-04-12 09:25:49
 * @FilePath: /ls2k0300_peripheral_library/lib/GPIO.h
 * @Description: GPIO类
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

#ifndef GPIO_H
#define GPIO_H

#include <cstring>
#include <fcntl.h>
#include <string>
#include <unistd.h>

class GPIO {
public:
    GPIO(int gpioNum_);
    ~GPIO(void);

    int getFileDescriptor(void) const; // 获取 GPIO 文件描述符
    bool setDirection(const std::string& direction); // 设置GPIO方向，out为输出，in为输入
    bool setValue(bool value); // 设置 GPIO 输出值
    bool readValue(void); // 读取 GPIO 输入值
    bool setEdge(const std::string& edge);

private:
    int gpioNum;
    int fd;
    std::string gpioPath;

    bool writeToFile(const std::string& path, const std::string& value);
};

#endif // GPIO_H
