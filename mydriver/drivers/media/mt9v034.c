/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-12-05 08:02:57
 * @LastEditors: Ilikara 3435193369@qq.com
 * @LastEditTime: 2025-02-14 09:55:09
 * @FilePath: /ls2k0300_peripheral_library/mydriver/drivers/media/mt9v034.c
 * @Description:
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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/serial_core.h>
#include <linux/irq.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/dma-iommu.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_dma.h>
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
#define CCER_CCnE(n) BIT((n) * 4 + 0)
#define CCER_CCnP(n) BIT((n) * 4 + 1)

#define CAM_GTIM_CH (3 - 1)

#define DMA_BASE 0x1612c000
#define DMA_ISR 0x00
#define DMA_IFCR 0x04
#define DMA_CCR(n) ((n) * 0x14 + 0x08)
#define DMA_CNDTR(n) ((n) * 0x14 + 0x0c)
#define DMA_CPAR(n) ((n) * 0x14 + 0x10)
#define DMA_CMAR(n) ((n) * 0x14 + 0x14)

/* CCR each bit */
#define CCR_EN BIT(0)
#define CCR_TCIE BIT(1)
#define CCR_HTIE BIT(2)
#define CCR_TEIE BIT(3)
#define CCR_DIR BIT(4)
#define CCR_CIRC BIT(5)
#define CCR_PINC BIT(6)
#define CCR_MINC BIT(7)
#define CCR_PSIZE BIT(8)
#define CCR_MSIZE BIT(10)
#define CCR_PL BIT(12)
#define CCR_MEM2MEM BIT(14)

struct mt9v034_camera
{
    u32 width, height;
    struct dma_slave_config dma_cfg;
    struct gpio_desc *gpio[8];
    struct platform_device *gpio_pdev, *uart_pdev;
    struct dma_chan *dma_chan;
    struct class *img_class;
    struct device *img_device;
    void __iomem *gtim_mmio_base, *gpio_mmio_base, *dma_mmio_base;
    u32 irq;
    u32 start_gpio;
    u32 depth;
    u32 count, img_size;
    u8 *target_buffer;
    dma_addr_t dma_handle;
    u32 major;
};

#define START_GPIO 16 // 使用gpio16-23

#define CCR_PL BIT(12)

#define GPIO_BASE 0x16104000
#define GPIO_OEN(n) (0x00 + (n) / 8 * 0x01)
#define GPIO_O(n) (0x10 + (n) / 8 * 0x01)
#define GPIO_I(n) (0x20 + (n) / 8 * 0x01)
#define GPIO_INT_EN(n) (0x30 + (n) / 8 * 0x01)
#define GPIO_INT_POL(n) (0x40 + (n) / 8 * 0x01)
#define GPIO_INT_EDGE(n) (0x50 + (n) / 8 * 0x01)
#define GPIO_INT_CLR(n) (0x60 + (n) / 8 * 0x01)

u8 mt9v034_image_buffer[120][188];
u8 *mt9v034_image;
u32 mt9v034_height, mt9v034_width;

static irqreturn_t mt9v034_vsync_isr(int irq, void *dev)
{
    struct mt9v034_camera *cam = (struct mt9v034_camera *)dev;

    u8 int_clr_reg, int_en_reg;
    u32 dier_reg;
    u32 ccr_reg, cndtr_reg;

    int_en_reg = readb(cam->gpio_mmio_base + GPIO_INT_EN(15));
    int_en_reg &= ~BIT(15 % 8); // 停止中断
    writeb(int_en_reg, cam->gpio_mmio_base + GPIO_INT_EN(15));

    int_clr_reg = readb(cam->gpio_mmio_base + GPIO_INT_CLR(0));
    int_clr_reg |= BIT(15); // 清中断
    writeb(int_clr_reg, cam->gpio_mmio_base + GPIO_INT_CLR(0));

    // 关闭dma
    // dmaengine_terminate_sync(cam->dma_chan);
    ccr_reg = readl(cam->dma_mmio_base + DMA_CCR(2));
    ccr_reg &= ~(CCR_EN);
    writel(ccr_reg, cam->dma_mmio_base + DMA_CCR(2));

    // 传出
    memcpy(mt9v034_image, cam->target_buffer, cam->width * cam->height);

    // 设置当前DMA传输的剩余数量，向下递减
    // dmaengine_prep_dma_cyclic(cam->dma_chan, (dma_addr_t)cam->target_buffer,
    // 			  cam->width * cam->height, 1, DMA_DEV_TO_MEM,
    // 			  0);
    cndtr_reg = cam->width * cam->height;
    writel(cndtr_reg, cam->dma_mmio_base + DMA_CNDTR(2)); // 传输数量

    // 使能dma
    // dma_async_issue_pending(cam->dma_chan);
    ccr_reg = readl(cam->dma_mmio_base + DMA_CCR(2));
    ccr_reg |= (CCR_EN);
    writel(ccr_reg, cam->dma_mmio_base + DMA_CCR(2));

    // 使能tim中断
    dier_reg = readl(cam->gtim_mmio_base + GTIM_DIER);
    dier_reg |= DIER_CCnDE(CAM_GTIM_CH); // DMA请求使能
    writel(dier_reg, cam->gtim_mmio_base + GTIM_DIER);
    return IRQ_HANDLED;
}

static int dma_init(struct platform_device *pdev, struct mt9v034_camera *cam)
{
    /*
    struct dma_async_tx_descriptor *desc;
    // 获取DMA通道
    cam->dma_chan = dma_request_slave_channel(&pdev->dev, "rx");
    dev_info(&pdev->dev, "dma_request returned %d", cam->dma_chan);
    if (!cam->dma_chan) {
        dev_err(&pdev->dev, "Failed to request DMA channel\n");
        return -ENOMEM;
    }

    memset(&cam->dma_cfg, 0, sizeof(&cam->dma_cfg));

    // 配置DMA的源地址和目标地址
    cam->dma_cfg.src_addr =
        (dma_addr_t)GPIO_BASE + GPIO_I(0); // 已知的并口地址 GPIO 0-7
    cam->dma_cfg.dst_addr = cam->dma_handle; // 图像缓存地址

    // 配置Slave DMA模式
    cam->dma_cfg.direction = DMA_DEV_TO_MEM;
    cam->dma_cfg.src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
    cam->dma_cfg.dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
    cam->dma_cfg.src_maxburst = 4;
    cam->dma_cfg.dst_maxburst = 4;

    // 关闭dma
    dmaengine_terminate_sync(cam->dma_chan);

    // 设置DMA通道为Slave模式
    dmaengine_slave_config(cam->dma_chan, &cam->dma_cfg);

    // 设置当前DMA传输的剩余数量，向下递减

    desc = dmaengine_prep_dma_cyclic(cam->dma_chan, cam->dma_handle,
                     cam->width * cam->height, 1,
                     DMA_DEV_TO_MEM, 0);
*/
    // 先用寄存器实现一个
    u32 cpar_reg, cmar_reg, cndtr_reg, ccr_reg;

    ccr_reg = 0;
    writel(ccr_reg, cam->dma_mmio_base + DMA_CCR(2)); // 停止dma2

    cpar_reg = GPIO_BASE + GPIO_I(START_GPIO);
    writel(cpar_reg, cam->dma_mmio_base + DMA_CPAR(2)); // 外设地址

    cmar_reg = cam->dma_handle;
    writel(cmar_reg, cam->dma_mmio_base + DMA_CMAR(2)); // 存储器地址

    cndtr_reg = cam->width * cam->height;
    writel(cndtr_reg, cam->dma_mmio_base + DMA_CNDTR(2)); // 传输数量

    ccr_reg = 0b11 * CCR_PL;                          // 最高优先级
    ccr_reg |= CCR_CIRC;                              // 循环模式
    ccr_reg &= ~CCR_PINC;                             // 外设地址非增量模式
    ccr_reg |= CCR_MINC;                              // 存储器地址增量模式
    ccr_reg &= ~(0b11 * CCR_MSIZE);                   // 8位
    ccr_reg &= ~(0b11 * CCR_PSIZE);                   // 8位
    ccr_reg &= ~(CCR_MEM2MEM);                        // 非内存到内存
    writel(ccr_reg, cam->dma_mmio_base + DMA_CCR(2)); // 配置dma2
    return 0;
}

static ssize_t mt9v034_file_read(struct file *flip, char __user *buf,
                                 size_t count, loff_t *f_pos)
{
    // struct mt9v034_camera *cam = flip->private_data;
    u32 image_size = mt9v034_height * mt9v034_width;

    if (*f_pos > count)
        return 0;
    if (count > image_size - *f_pos)
        count = image_size - *f_pos;

    if (*f_pos >= image_size || count == 0)
        return 0; // End of file

    if (copy_to_user(buf, mt9v034_image + *f_pos, count))
        return -EFAULT;

    *f_pos += count;
    return count;
}

// static int mt9v034_file_open(struct inode *inode, struct file *filp)
// {
// 	struct platform_device *pdev;
// 	struct mt9v034_camera *cam;

// 	pdev = container_of(&inode->i_cdev->dev, struct platform_device, dev);
// 	cam = platform_get_drvdata(pdev);

// 	// 将my_device结构指针赋值给file的private_data
// 	filp->private_data = cam;

// 	return 0;
// }

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = mt9v034_file_read,
    // .open = mt9v034_file_open,
};

static int mt9v034_class_init(struct platform_device *pdev,
                              struct mt9v034_camera *cam)
{
#define MT9V034_DEVICE_NAME "test_mt9v034"
#define MT9V034_CLASS_NAME "test_mt9v034_class"
    cam->img_class = NULL;
    cam->img_device = NULL;

    cam->major = register_chrdev(0, MT9V034_DEVICE_NAME, &fops);
    if (cam->major < 0)
    {
        dev_err(&pdev->dev, "failed to register_chrdev\n");
        return cam->major;
    }
    dev_info(&pdev->dev, "MT9V034 major requested %d\n", cam->major);

    cam->img_class = class_create(THIS_MODULE, MT9V034_CLASS_NAME);
    if (IS_ERR(cam->img_class))
    {
        unregister_chrdev(cam->major, MT9V034_DEVICE_NAME);
        return PTR_ERR(cam->img_class);
    }

    cam->img_device =
        device_create(cam->img_class, NULL, MKDEV(cam->major, 0), NULL,
                      MT9V034_DEVICE_NAME);
    if (IS_ERR(cam->img_device))
    {
        class_destroy(cam->img_class);
        unregister_chrdev(cam->major, MT9V034_DEVICE_NAME);
        return PTR_ERR(cam->img_device);
    }

    dev_info(&pdev->dev, "MT9V034: loaded on major %d\n", cam->major);
    return 0;
}

// data-pins = <&gpa0 4 8 GPIO_ACTIVE_HIGH>;
// uart = <&cpu_uart1>;

static int mt9v034_probe(struct platform_device *pdev)
{
    struct mt9v034_camera *cam;
    struct device_node *np = pdev->dev.of_node, *gpio_np, *uart_np;

    struct of_phandle_args data_pins_args;
    struct resource *mem;

    u32 err;
    u8 oen_reg;
    u8 int_pol_reg;
    u8 int_edge_reg;
    u8 int_en_reg;
    u32 ccmr_reg;
    u32 ccer_reg;

    dev_info(&pdev->dev, "mt9v034 init start");

    cam = devm_kzalloc(&pdev->dev, sizeof(*cam), GFP_KERNEL);

    if (!cam)
    {
        dev_err(&pdev->dev, "failed to allocate memory\n");
        return -ENOMEM;
    }
    cam->height = mt9v034_height = 120;
    cam->width = mt9v034_width = 188;
    // cam->target_buffer =
    // 	dma_alloc_coherent(&pdev->dev, cam->height * cam->width,
    // 			   &cam->dma_handle, GFP_KERNEL);
    cam->target_buffer = &mt9v034_image_buffer[0][0];
    cam->dma_handle = dma_map_single(&pdev->dev, mt9v034_image_buffer, 120 * 188, DMA_DEV_TO_MEM);
    dev_info(&pdev->dev, "mt9v034 dma handle: %0x\n", cam->dma_handle);
    dev_info(&pdev->dev, "mt9v034 target buffer: %0x\n",
             cam->target_buffer);

    if (!cam->target_buffer)
    {
        dev_err(&pdev->dev, "failed to allocate dma memory\n");
        return -ENOMEM;
    }

    // mt9v034_image =
    // 	devm_kzalloc(&pdev->dev, cam->height * cam->width, GFP_KERNEL);
    // mt9v034_image = cam->target_buffer;
    mt9v034_image = &mt9v034_image_buffer[0][0];

    dev_info(&pdev->dev, "mt9v034 image buffer: %0x\n", mt9v034_image);

    if (!cam->target_buffer)
    {
        dev_err(&pdev->dev, "failed to allocate image memory\n");
        return -ENOMEM;
    }
    // cam->target_buffer = devm_kzalloc(&pdev->dev, cam->height * cam->width, GFP_KERNEL);

    // 获取GTIM基地址
    // mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    // if (!mem) {
    // 	dev_err(&pdev->dev, "no mem resource?\n");
    // 	return -ENODEV;
    // }
    // cam->gtim_mmio_base = devm_ioremap_resource(&pdev->dev, mem);
    // if (!cam->gtim_mmio_base) {
    // 	dev_err(&pdev->dev, "gtim mmio_base is null\n");
    // 	return -ENOMEM;
    // }
    cam->gtim_mmio_base = devm_ioremap(&pdev->dev, 0x16119000, 0x1000);
    if (!cam->gtim_mmio_base)
    {
        dev_err(&pdev->dev, "gtim mmio_base is null\n");
        return -ENOMEM;
    }

    cam->dma_mmio_base = devm_ioremap(&pdev->dev, DMA_BASE, 0x4000);
    if (!cam->dma_mmio_base)
    {
        dev_err(&pdev->dev, "dma mmio_base is null\n");
        return -ENOMEM;
    }

    // 获取GPIO基地址
    cam->gpio_mmio_base = devm_ioremap(&pdev->dev, GPIO_BASE, 0x80);
    if (!cam->gpio_mmio_base)
    {
        dev_err(&pdev->dev, "gpio mmio_base is null\n");
        return -ENOMEM;
    }

    // // 获取 GPIO 范围
    // if (of_parse_phandle_with_fixed_args(np, "data-pins", 4, 0,
    // 				     &data_pins_args) < 0) {
    // 	dev_err(&pdev->dev, "Failed to parse data-pins\n");
    // 	return -EINVAL;
    // }

    // 获取gpio设备
    // gpio_np = data_pins_args.np;
    // cam->gpio_pdev = of_find_device_by_node(gpio_np);
    // cam->gpio_pdev->id

    // cam->start_gpio = data_pins_args.args[1]; // 起始 GPIO 编号
    // cam->depth = data_pins_args.args[2]; // GPIO 数量

    // if (cam->start_gpio + cam->depth > 16) {
    // 	dev_err(&pdev->dev, "GPIO out of range\n");
    // 	return -EINVAL;
    // }
    // 设置gpio为输入
    // for (u32 i = 0; i < cam->depth; ++i)
    // {
    //     cam->gpio[i] = gpiod_get_index(gpio_np, NULL, 1, GPIOD_IN);
    //     if (IS_ERR(cam->gpio[i]))
    //     {
    //         dev_err(&pdev->dev, "Failed to get GPIO %d\n", i);
    //     }
    // }
    oen_reg = readb(cam->gpio_mmio_base + GPIO_OEN(START_GPIO));
    oen_reg |= 0xFF; // GPIO16-23设置为输入
    writeb(oen_reg, cam->gpio_mmio_base + GPIO_OEN(START_GPIO));

    // uart_np = of_parse_phandle(np, "uart", 0);
    // cam->uart_pdev = of_find_device_by_node(uart_np);
    // struct uart_port *port = platform_get_drvdata(cam->uart

    // // 配置波特率
    // uart_set_options(port, NULL, 9600, 'n', 8, 0);

    // uart_write(port, (u_char *));
    // dev_info(&pdev->dev, "GPIO range: start=%d, count=%d\n",
    // 	 cam->start_gpio, cam->depth);

    // 初始化DMA
    dev_info(&pdev->dev, "mt9v034 dma init start\n");
    err = dma_init(pdev, cam);
    if (err != 0)
    {
        return err;
    }

    // 为PCLK启用GTIM_CH3的捕获中断DMA请求，通道号定义在CAM_GTIM_CH
    dev_info(&pdev->dev, "mt9v034 gtim init start\n");

    writel(0xFFFF, cam->gtim_mmio_base + GTIM_ARR);
    writel(1, cam->gtim_mmio_base + GTIM_CR1);

    ccmr_reg = readl(cam->gtim_mmio_base + GTIM_CCMR(CAM_GTIM_CH));
    ccmr_reg &= ~(0b11 * CCMR_CCnS(CAM_GTIM_CH));
    ccmr_reg |= (0b01 * CCMR_CCnS(CAM_GTIM_CH)); // 输入模式，IC3=>TI3
    writel(ccmr_reg, cam->gtim_mmio_base + GTIM_CCMR(CAM_GTIM_CH));

    ccer_reg = readl(cam->gtim_mmio_base + GTIM_CCER);
    ccer_reg &= ~CCER_CCnP(CAM_GTIM_CH); // 上升沿触发
    ccer_reg |= CCER_CCnE(CAM_GTIM_CH);  // 捕获使能
    writel(ccer_reg, cam->gtim_mmio_base + GTIM_CCER);

    // 为Vsync(GPIO15)启用GPIO中断 下降沿触发
    dev_info(&pdev->dev, "mt9v034 vsync init start\n");

    oen_reg = readb(cam->gpio_mmio_base + GPIO_OEN(15));
    oen_reg |= BIT(15 % 8); // 输入
    writeb(oen_reg, cam->gpio_mmio_base + GPIO_OEN(15));

    // int_pol_reg = readb(cam->gpio_mmio_base + GPIO_INT_POL(15));
    // int_pol_reg &= ~BIT(15 % 8); // 下降沿中断
    // writeb(int_pol_reg, cam->gpio_mmio_base + GPIO_INT_POL(15));

    // int_edge_reg = readb(cam->gpio_mmio_base + GPIO_INT_EDGE(15));
    // int_edge_reg |= BIT(15 % 8); // 边沿触发
    // writeb(int_edge_reg, cam->gpio_mmio_base + GPIO_INT_EDGE(15));

    // 53为GPIO00-15的中断号，假设使用gpio15中断

    // 配置中断回调 需配置 interrupts=<53>;
    // dev_info(&pdev->dev, "mt9v034 irq init start\n");
    // cam->irq = platform_get_irq(pdev, 0);
    // if (cam->irq <= 0) {
    // 	dev_err(&pdev->dev, "no irq resource?\n");
    // 	return -ENODEV;
    // }
    // cam->irq = 53;
    // dev_info(&pdev->dev, "mt9v034 irq get %u\n", cam->irq);

    // err = request_irq(cam->irq, mt9v034_vsync_isr, IRQF_SHARED, "camera_interrupts",
    // 		  cam); // 不确定
    // dev_info(&pdev->dev, "mt9v034 irq get %u\n", err);

    // if (err){
    // 	dev_err(&pdev->dev, "failure requesting irq %d\n", err);
    // 	return err;
    // }

    // 初始化字符设备
    dev_info(&pdev->dev, "mt9v034 class init start\n");
    mt9v034_class_init(pdev, cam);
    dev_info(&pdev->dev, "mt9v034 class init complete\n");

    platform_set_drvdata(pdev, cam);

    // 等待场同步
    while (1)
    {
        if (!(readw(cam->gpio_mmio_base + GPIO_I(15)) & BIT(15 % 8)))
        {
            u32 ccr_reg = readl(cam->dma_mmio_base + DMA_CCR(2));
            ccr_reg |= (CCR_EN);
            writel(ccr_reg, cam->dma_mmio_base + DMA_CCR(2));
            u32 dier_reg = readl(cam->gtim_mmio_base + GTIM_DIER);
            dier_reg |= DIER_CCnDE(CAM_GTIM_CH); // DMA请求使能
            writel(dier_reg, cam->gtim_mmio_base + GTIM_DIER);
            break;
        }
    }
    int_en_reg = readb(cam->gpio_mmio_base + GPIO_INT_EN(15));
    int_en_reg |= BIT(15 % 8); // 使能中断
    writeb(int_en_reg, cam->gpio_mmio_base + GPIO_INT_EN(15));

    dev_info(&pdev->dev, "mt9v034 init complete\n");
    return 0;
}

static int mt9v034_remove(struct platform_device *pdev)
{
    struct mt9v034_camera *cam = platform_get_drvdata(pdev);
    int err;
    if (!cam)
        return -ENODEV;

    // 注销字符设备
    device_destroy(cam->img_class, MKDEV(cam->major, 0));
    class_destroy(cam->img_class);
    unregister_chrdev(cam->major, MT9V034_DEVICE_NAME);
    //...

    if (err < 0)
        return err;

    return 0;
}

#ifdef CONFIG_OF
static struct of_device_id ls_mt9v034_id_table[] = {
    {.compatible = "loongson,ls-mt9v034"},
    {},
};
MODULE_DEVICE_TABLE(of, ls_mt9v034_id_table);
#endif

static struct platform_driver ls_mt9v034_driver = {
    .driver = {
        .name = "ls-mt9v034",
        .owner = THIS_MODULE,
        .bus = &platform_bus_type,
#ifdef CONFIG_OF
        .of_match_table = of_match_ptr(ls_mt9v034_id_table),
#endif
    },
    .probe = mt9v034_probe,
    .remove = mt9v034_remove,
};
module_platform_driver(ls_mt9v034_driver);

MODULE_AUTHOR("Ilikara <3435193369@qq.com>");
MODULE_DESCRIPTION("Loongson Gtimer Pwm Driver");
MODULE_LICENSE("GPL");