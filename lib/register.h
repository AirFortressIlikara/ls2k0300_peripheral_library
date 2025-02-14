/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-12-01 03:56:32
 * @LastEditors: Ilikara 3435193369@qq.com
 * @LastEditTime: 2025-02-14 09:54:47
 * @FilePath: /ls2k0300_peripheral_library/lib/register.h
 * @Description: 寄存器操作
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
#ifndef REGISTER_H
#define REGISTER_H

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

#define PAGE_SIZE 0x10000

#define REG_READ(addr) (*(volatile uint32_t *)(addr))
#define REG_WRITE(addr, val) (*(volatile uint32_t *)(addr) = (val))

void *map_register(uint32_t physical_address, size_t size);

#endif
