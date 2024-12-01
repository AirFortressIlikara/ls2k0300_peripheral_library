/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-12-01 03:57:13
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2024-12-01 04:24:14
 * @FilePath: /ls2k0300_peripheral_library/src/register.cpp
 * @Description: 寄存器操作
 *
 * Copyright (c) 2024 by ilikara 3435193369@qq.com, All Rights Reserved.
 */
#include "register.h"

void *map_register(uint32_t physical_address, size_t size)
{
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd == -1)
    {
        perror("Failed to open /dev/mem");
        exit(EXIT_FAILURE);
    }

    void *mapped_addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, physical_address & ~(PAGE_SIZE - 1));
    if (mapped_addr == MAP_FAILED)
    {
        perror("Failed to map memory");
        close(mem_fd);
        exit(EXIT_FAILURE);
    }

    close(mem_fd);

    return (void *)((uintptr_t)mapped_addr + (physical_address & (PAGE_SIZE - 1)));
}