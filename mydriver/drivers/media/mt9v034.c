/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-12-05 08:02:57
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2024-12-05 16:41:27
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
#include <linux/dmaengine.h>
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
#define GTIM_CCMR(n) 0x18 + ((n) / 2) * 0x04
#define GTIM_CCER 0x20
#define GTIM_CNT 0x24
#define GTIM_PSC 0x28
#define GTIM_ARR 0x2C
#define GTIM_CCR(n) 0x34 + (n) * 0x04
#define GTIM_INSTA 0x50

/* CR1 each bit */
#define CR1_CEN BIT(0)

/* DIER each bit */
#define DIER_UIE BIT(0)
#define DIER_CCnIE(n) BIT(1 + (n))
#define DIER_TIE BIT(6)
#define DIER_UDE BIT(8)
#define DIER_CCnDE(n) BIT(9 + (n))
#define DIER_TDE BIT(14)

/* SR each bit */
#define SR_UIF BIT(0)
#define SR_CCnIF(n) BIT((n) + 1)
#define SR_TIF BIT(6)
#define SR_CCnOF(n) BIT((n) + 9)

/* EGR each bit */
#define EGR_UG BIT(0)

/* CCMR each bit */
#define CCMR_CCnS(n) BIT((n) % 2 * 8 + 0)

/* CCER each bit */
#define CCER_CCnE BIT(pwm->hwpwm * 4 + 0)
#define CCER_CCnP BIT(pwm->hwpwm * 4 + 1)

#define CAM_GTIM_CH (3 - 1)

struct mt9v034_camera
{
    struct gpio_desc *gpio[10];
    struct platform_device *gpio_pdev, *uart_pdev;
    struct dma_chan *dma_chan;
    void __iomem *mmio_base;
    u32 irq;
    u32 start_gpio;
    u32 depth;
    u32 count, img_size;
    u8 buffer[][], image[][], *it;
};

#define GPIO_BASE 0x16104000
#define GPIO_I 0x20
// 假设为GPIO04-11 vsync为GPIO 0
static irqreturn_t
mt9v034_isr(int irq, void *dev)
{
    struct mt9v034_camera *cam = (struct mt9v034_camera *)dev;
    *cam->it = (readl(GPIO_BASE + GPIO_I + 0x07) >> cam->start_gpio);
    cam->it++;
    cam->count++;
    if (!(readl(GPIO_BASE + GPIO_I + 0x07) & BIT(0)) || cam->count > cam->img_size)
    {
        cam->it = cam->buffer;
        memcpy(cam->image, cam->buffer, cam->img_size);
    }
    readl(cam->mmio_base + GTIM_CCR(CAM_GTIM_CH));
    return IRQ_HANDLED;
}

// data-pins = <&gpa0 4 8 GPIO_ACTIVE_HIGH>;
// uart = <&cpu_uart1>;
static int
mt9v034_probe(struct platform_device *pdev)
{
    struct mt9v034_camera *cam;
    struct device_node *np = pdev->dev.of_node, *gpio_np, *uart_np;

    struct of_phandle_args data_pins_args;
    struct resource *mem;

    cam = devm_kzalloc(&pdev->dev, sizeof(*cam), GFP_KERNEL);
    if (!cam)
    {
        dev_err(&pdev->dev, "failed to allocate memory\n");
        return -ENOMEM;
    }

    // 获取GTIM基地址
    mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!mem)
    {
        dev_err(&pdev->dev, "no mem resource?\n");
        return -ENODEV;
    }
    cam->mmio_base = devm_ioremap_resource(&pdev->dev, mem);
    if (!cam->mmio_base)
    {
        dev_err(&pdev->dev, "mmio_base is null\n");
        return -ENOMEM;
    }

    // 获取 GPIO 范围
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
    // uart_set_options(port, NULL, 9600, 'n', 8, 0);

    // uart_write(port, (u_char *));
    dev_info(&pdev->dev, "GPIO range: start=%d, count=%d\n", start_gpio, ngpios);

    // DMA初始化
    cam->dma_chan = dma_request_slave_channel(&pdev->dev, "CAM_PCLK");
    if (!dma_chan)
    {
        dev_err(&pdev->dev, "Failed to request DMA channel\n");
        return -ENODEV;
    }
    struct dma_slave_config config = {
        .direction = DMA_FROM_DEVICE,                // 或者 DMA_TO_DEVICE
        .src_addr = GPIO_BASE + GPIO_I + 0x07,       // 使用GPIO00-07
        .dst_addr = cam->buffer,                     // 目标地址
        .src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE, // 源地址宽度
        .dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE, // 目标地址宽度
        .src_maxburst = 16,                          // 源端最大突发长度
        .dst_maxburst = 16,                          // 目标端最大突发长度
        .device_fc = false,                          // 是否启用流控
        .slave_id = 0,                               // 如果需要指定从设备ID
    };
    if (dmaengine_slave_config(cam->dma_chan, &config))
    {
        dev_err(dev, "Failed to configure DMA channel\n");
        return -EINVAL;
    }
    struct scatterlist sg;
    sg_init_table

    // 为PCLK启用GTIM_CH3的捕获中断，通道号定义在CAM_GTIM_CH

    u32 ccmr_reg = readl(cam->mmio_base + GTIM_CCMR(CAM_GTIM_CH));
    ccmr_reg &= ~(0b11 * CCMR_CCnS(CAM_GTIM_CH));
    ccmr_reg |= (0b01 * CCMR_CCnS(CAM_GTIM_CH)); // 输入模式，IC3=>TI3
    writel(ccmr_reg, cam->mmio_base + GTIM_CCMR(CAM_GTIM_CH));

    u32 ccer_reg = readl(cam->mmio_base + GTIM_CCER);
    ccer_reg &= ~CCER_CCnP(CAM_GTIM_CH); // 上升沿触发
    ccer_reg |= CCER_CCnE(CAM_GTIM_CH);  // 捕获使能
    writel(ccer_reg, cam->mmio_base + GTIM_CCER);

    u32 dier_reg = readl(cam->mmio_base + GTIM_DIER);
    dier_reg |= DIER_CCnIE(CAM_GTIM_CH); // 中断使能
    writel(dier_reg, cam->mmio_base + GTIM_DIER);

    // 为Vsync启用GPIO中断 下降沿触发

    // 配置中断回调 需配置 interrupts=<26>;
    // 26为GTIM的中断号
    cam->irq = platform_get_irq(pdev, 0);
    if (cam->irq <= 0)
    {
        dev_err(&pdev->dev, "no irq resource?\n");
        return -ENODEV;
    }

    err = request_irq(cam->irq, mt9v034_isr, IRQF_TRIGGER_RISING, "camera_interrupts", cam); // 不确定

    if (err)
        dev_err(&pdev->dev, "failure requesting irq %d\n", err);

    return 0;
}