/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-12-05 08:02:57
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2024-12-05 12:49:03
 * @FilePath: /ls2k0300_peripheral_library/mydriver/drivers/media/mt9v034.c
 * @Description:
 *
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/serial_core.h>
#include <linux/tty.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>

/* counter offest */
#define GTIM_CR1 0x00
#define GTIM_CR2 0x04
#define GTIM_SMCR 0x08
#define GTIM_DIER 0x0C
#define GTIM_SR 0x10
#define GTIM_EGR 0x14
#define GTIM_CCMR(n) 0x18 + (n / 2) * 0x04
#define GTIM_CCER 0x20
#define GTIM_CNT 0x24
#define GTIM_PSC 0x28
#define GTIM_ARR 0x2C
#define GTIM_CCR1 0x34
#define GTIM_CCR(n) 0x34 + (n) * 0x04
#define GTIM_INSTA 0x50

/* CR1 each bit */
#define CR1_CEN BIT(0)

/* DIER each bit*/
#define DIER_UIE BIT(0)
#define DIER_CCnIE(n) BIT(1 + n)
#define DIER_TIE BIT(6)
#define DIER_UDE BIT(8)
#define DIER_CCnDE(n) BIT(9 + n)
#define DIER_TDE BIT(14)

/* EGR each bit */
#define EGR_UG BIT(0)

/* CCMR each bit */

/* CCER each bit */
#define CCER_CCnE BIT(pwm->hwpwm * 4 + 0)
#define CCER_CCnP BIT(pwm->hwpwm * 4 + 1)

struct mt9v034_camera
{
    struct gpio_desc *gpio[10];
    struct platform_device *gpio_pdev, *uart_pdev;
    u32 irq;
    u32 start_gpio;
    u32 depth;
}

// data-pins = <&gpa0 4 8 GPIO_ACTIVE_HIGH>;
// uart = <&cpu_uart1>;

static irqreturn_t
mt9v034_isr(int irq, void *dev)
{
}

static int
mt9v034_probe(struct platform_device *pdev)
{
    struct mt9v034_camera *cam;
    struct device_node *np = pdev->dev.of_node, *gpio_np, *uart_np;

    struct of_phandle_args data_pins_args;

    // 从设备树获取 GPIO 范围
    if (of_parse_phandle_with_fixed_args(np, "data-pins", 4, 0, &data_pins_args) < 0)
    {
        dev_err(&pdev->dev, "Failed to parse data-pins\n");
        return -EINVAL;
    }

    // 获取gpio设备
    gpio_np = data_pins_args.np;
    cam->gpio_pdev = of_find_device_by_node(gpio_np);
    // cam->gpio_pdev->id

    cam->start_gpio = data_pins_args.args[1]; // 起始 GPIO 编号
    cam->depth = data_pins_args.args[2];      // GPIO 数量

    if (cam->start_gpio + cam->depth > 16)
    {
        dev_err(&pdev->dev, "GPIO out of range\n");
        return -EINVAL;
    }
    // 设置gpio为输入
    for (u32 i = 0; i < cam->depth; ++i)
    {
        cam->gpio[i] = gpiod_get_index(gpio_np, NULL, 1, GPIOD_IN);
        if (IS_ERR(cam->gpio[i]))
        {
            dev_err("Failed to get GPIO %d", i);
        }
    }

    uart_np = of_parse_phandle(np, "uart", 0);
    cam->uart_pdev = of_find_device_by_node(uart_np);
    struct uart_port *port = platform_get_drvdata(cam->uart_pdev);

    // 配置波特率
    uart_set_options(port, NULL, 9600, 'n', 8, 0);

    uart_write(port, (u_char *));
    dev_info(&pdev->dev, "GPIO range: start=%d, count=%d\n", start_gpio, ngpios);

    // 请求并操作每个 GPIO
    for (i = 0; i < ngpios; i++)
    {
        int gpio_number = start_gpio + i;

        if (gpio_request(gpio_number, "my-gpio"))
        {
            dev_err(&pdev->dev, "Failed to request GPIO %d\n", gpio_number);
            continue;
        }

        gpio_direction_input(gpio_number); // 设置为输入模式

        // 读取 GPIO 状态
        int value = gpio_get_value(gpio_number);
        dev_info(&pdev->dev, "GPIO %d value: %d\n", gpio_number, value);

        gpio_free(gpio_number); // 释放 GPIO
    }

    of_node_put(gpio_spec.np); // 释放 phandle

    // 启用GTIM的捕获中断

    // 中断回调 需配置 interrupts=<26>;
    // 26为GTIM的中断号
    u32 irq;

    irq = platform_get_irq(pdev, 0);
    if (irq <= 0)
    {
        dev_err(&pdev->dev, "no irq resource?\n");
        return -ENODEV;
    }
    cam->irq = irq;

    err = request_irq(cam->irq, mt9v034_isr, IRQF_TRIGGER_RISING, "camera_interrupts", cam);

    if (err)
        dev_err(&pdev->dev, "failure requesting irq %d\n", err);

    return 0;
}