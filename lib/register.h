/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-12-01 03:56:32
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2024-12-01 04:24:13
 * @FilePath: /ls2k0300_peripheral_library/lib/register.h
 * @Description: 寄存器操作
 *
 * Copyright (c) 2024 by ilikara 3435193369@qq.com, All Rights Reserved.
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
