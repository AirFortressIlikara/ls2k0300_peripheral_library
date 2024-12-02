/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-12-02 07:23:11
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2024-12-02 09:06:56
 * @FilePath: /ls2k0300_peripheral_library/mydriver/drivers/pwm/pwm-ls-gtim.c
 * @Description:
 *
 * Copyright (c) 2024 by ilikara 3435193369@qq.com, All Rights Reserved.
 */
#include <linux/acpi.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

#include <linux/clk.h>
#include <linux/err.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/atmel_tc.h>
#include <linux/pwm.h>
#include <linux/of_device.h>
#include <linux/slab.h>

/* counter offest */
#define GTIM_CR1 0x00
#define GTIM_CR2 0x04
#define GTIM_SMCR 0x08
#define GTIM_DIER 0x0C
#define GTIM_SR 0x10
#define GTIM_EGR 0x14
#define GTIM_CCMR1 0x18
#define GTIM_CCMR2 0x1C
#define GTIM_CCER 0x20
#define GTIM_CNT 0x24
#define GTIM_PSC 0x28
#define GTIM_ARR 0x2C
#define GTIM_CCR1 0x34
#define GTIM_CCR2 0x38
#define GTIM_CCR3 0x3C
#define GTIM_CCR4 0x40
#define GTIM_INSTA 0x50

/* CTRL counter each bit */
#define CTRL_EN BIT(pwm->hwpwm * 4 + 0)
#define CTRL_OE BIT(3)
#define CTRL_SINGLE BIT(4)
#define CTRL_INTE BIT(5)
#define CTRL_INT BIT(6)
#define CTRL_RST BIT(7)
#define CTRL_CAPTE BIT(8)
#define CTRL_INVERT BIT(pwm->hwpwm * 4 + 1)
#define CTRL_DZONE BIT(10)

#define to_ls_pwm_gtim_chip(_chip) container_of(_chip, struct ls_pwm_gtim_chip, chip)
#define NS_IN_HZ (1000000000UL)
#define CPU_FRQ_PWM (50000000UL)

struct ls_pwm_gtim_chip
{
	struct pwm_chip chip;
	void __iomem *mmio_base;
	/* following registers used for suspend/resume */
	u32 irq;
	u32 ccer_reg;
	u64 clock_frequency;
};

static int ls_pwm_gtim_set_polarity(struct pwm_chip *chip,
									struct pwm_device *pwm,
									enum pwm_polarity polarity)
{
	struct ls_pwm_gtim_chip *ls_pwm = to_ls_pwm_gtim_chip(chip);
	u16 val;

	val = readl(ls_pwm->mmio_base + GTIM_CCER);
	switch (polarity)
	{
	case PWM_POLARITY_NORMAL:
		val &= ~CTRL_INVERT;
		break;
	case PWM_POLARITY_INVERSED:
		val |= CTRL_INVERT;
		break;
	default:
		break;
	}
	writel(val, ls_pwm->mmio_base + GTIM_CCER);
	return 0;
}

static void ls_pwm_gtim_disable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct ls_pwm_gtim_chip *ls_pwm = to_ls_pwm_gtim_chip(chip);
	u32 ret;

	if (pwm->state.polarity == PWM_POLARITY_NORMAL)
		writel(ls_pwm->full_buffer_reg, ls_pwm->mmio_base + LOW_BUFFER);
	else if (pwm->state.polarity == PWM_POLARITY_INVERSED)
		writel(0, ls_pwm->mmio_base + LOW_BUFFER);

	ret = readl(ls_pwm->mmio_base + CTRL);
	ret &= ~CTRL_EN;
	writel(ret, ls_pwm->mmio_base + CTRL);
}

static int ls_pwm_gtim_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct ls_pwm_gtim_chip *ls_pwm = to_ls_pwm_gtim_chip(chip);
	int ret;

	writel(ls_pwm->low_buffer_reg, ls_pwm->mmio_base + LOW_BUFFER);
	writel(ls_pwm->full_buffer_reg, ls_pwm->mmio_base + FULL_BUFFER);

	ret = readl(ls_pwm->mmio_base + CTRL);
	ret |= CTRL_EN;
	writel(ret, ls_pwm->mmio_base + CTRL);
	return 0;
}

static int ls_pwm_gtim_config(struct pwm_chip *chip, struct pwm_device *pwm,
							  int duty_ns, int period_ns)
{
	struct ls_pwm_gtim_chip *ls_pwm = to_ls_pwm_gtim_chip(chip);
	unsigned int period_reg, duty_reg;
	unsigned long long val0, val1;

	if (period_ns > NS_IN_HZ || duty_ns > NS_IN_HZ)
		return -ERANGE;

	/*
	 * period_reg = period_ns/(NSEC_PER_SEC/ls_pwm->clock_frequency);
	 * period_reg = period_ns * ls_pwm->clock_frequency / NSEC_PER_SEC;
	 */
	val0 = ls_pwm->clock_frequency * period_ns;
	do_div(val0, NSEC_PER_SEC);
	period_reg = val0 < 1 ? 1 : val0;

	val1 = ls_pwm->clock_frequency * duty_ns;
	do_div(val1, NSEC_PER_SEC);
	duty_reg = val1 < 1 ? 1 : val1;

	writel(duty_reg, ls_pwm->mmio_base + GTIM_CCR1 + pwm->hwpwm * 0x04);
	writel(period_reg, ls_pwm->mmio_base + GTIM_ARR);
	return 0;
}

static unsigned int ls_pwm_gtim_reg_to_ns(struct ls_pwm_gtim_chip *ls_pwm, unsigned int reg)
{
	unsigned long long val;

	val = reg * NSEC_PER_SEC;
	do_div(val, ls_pwm->clock_frequency);
	return val;
}

void ls_pwm_gtim_get_state(struct pwm_chip *chip, struct pwm_device *pwm,
						   struct pwm_state *state)
{
	struct ls_pwm_gtim_chip *ls_pwm = to_ls_pwm_gtim_chip(chip);
	unsigned int period_reg, duty_reg;
	u32 ccer_reg;

	/*
	 * period_ns = period_reg *NSEC_PER_SEC /ls_pwm->clock_frequency.
	 */
	period_reg = readl(ls_pwm->mmio_base + GTIM_ARR);
	state->period = ls_pwm_gtim_reg_to_ns(ls_pwm, period_reg);

	duty_reg = readl(ls_pwm->mmio_base + GTIM_CCR1 + 0x04 * pwm->hwpwm);
	state->duty_cycle = ls_pwm_gtim_reg_to_ns(ls_pwm, duty_reg);

	ccer_reg = readl(ls_pwm->mmio_base + GTIM_CCER);
	state->polarity = (ccer_reg & CTRL_INVERT) ? PWM_POLARITY_INVERSED
											   : PWM_POLARITY_NORMAL;
	state->enabled = (ccer_reg & CTRL_EN) ? true : false;

	ls_pwm->ccer_reg = ccer_reg;
}

static const struct pwm_ops ls_pwm_gtim_ops = {
	.config = ls_pwm_gtim_config,
	.set_polarity = ls_pwm_gtim_set_polarity,
	.enable = ls_pwm_gtim_enable,
	.disable = ls_pwm_gtim_disable,
	.get_state = ls_pwm_gtim_get_state,
	.owner = THIS_MODULE,
};
static irqreturn_t pwm_ls2x_isr(int irq, void *dev)
{
	int ret;
	struct ls_pwm_gtim_chip *ls_pwm = (struct ls_pwm_gtim_chip *)dev;
	ret = readl(ls_pwm->mmio_base + CTRL);
	ret |= CTRL_INT;

	writel(ret, ls_pwm->mmio_base + CTRL);

	return IRQ_HANDLED;
}

static int ls_pwm_gtim_probe(struct platform_device *pdev)
{
	struct ls_pwm_gtim_chip *pwm;
	struct resource *mem;
	int err;
	struct device_node *np = pdev->dev.of_node;
	u32 clk;
	u32 irq;

	irq = platform_get_irq(pdev, 0);
	if (irq <= 0)
	{
		dev_err(&pdev->dev, "no irq resource?\n");
		return -ENODEV;
	}

	pwm = devm_kzalloc(&pdev->dev, sizeof(*pwm), GFP_KERNEL);
	if (!pwm)
	{
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}

	pwm->chip.dev = &pdev->dev;
	pwm->chip.ops = &ls_pwm_gtim_ops;
	pwm->chip.base = -1;
	pwm->chip.npwm = 4;

	if (!(of_property_read_u32(np, "clock-frequency", &clk)))
		pwm->clock_frequency = clk;
	else
		pwm->clock_frequency = CPU_FRQ_PWM;

	dev_info(&pdev->dev, "pwm->clock_frequency=%llu", pwm->clock_frequency);

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem)
	{
		dev_err(&pdev->dev, "no mem resource?\n");
		return -ENODEV;
	}
	pwm->mmio_base = devm_ioremap_resource(&pdev->dev, mem);
	if (!pwm->mmio_base)
	{
		dev_err(&pdev->dev, "mmio_base is null\n");
		return -ENOMEM;
	}
	pwm->irq = irq;

	if (pdev->dev.of_node && of_device_is_compatible(pdev->dev.of_node, "loongson,ls300-pwm"))
	{
		err = request_irq(pwm->irq, pwm_ls2x_isr, IRQF_SHARED, "pwm_interrupts", pwm);
	}
	else
	{
		err = request_irq(pwm->irq, pwm_ls2x_isr, IRQF_TRIGGER_FALLING, "pwm_interrupts", pwm);
	}

	if (err)
		dev_err(&pdev->dev, "failure requesting irq %d\n", err);

	err = pwmchip_add(&pwm->chip);
	if (err < 0)
	{
		dev_err(&pdev->dev, "pwmchip_add() failed: %d\n", err);
		return err;
	}

	platform_set_drvdata(pdev, pwm);
	dev_dbg(&pdev->dev, "pwm probe successful\n");
	return 0;
}

static int ls_pwm_gtim_remove(struct platform_device *pdev)
{
	struct ls_pwm_gtim_chip *pwm = platform_get_drvdata(pdev);
	int err;
	if (!pwm)
		return -ENODEV;
	free_irq(pwm->irq, NULL);
	err = pwmchip_remove(&pwm->chip);
	if (err < 0)
		return err;

	return 0;
}

#ifdef CONFIG_OF
static struct of_device_id ls_pwm_gtim_id_table[] = {
	{.compatible = "loongson,ls7a-pwm"},
	{.compatible = "loongson,ls-pwm"},
	{.compatible = "loongson,ls2k-pwm"},
	{.compatible = "loongson,ls300-pwm"},
	{},
};
MODULE_DEVICE_TABLE(of, ls_pwm_gtim_id_table);
#endif

#ifdef CONFIG_PM_SLEEP
static int ls_pwm_gtim_suspend(struct device *dev)
{
	struct ls_pwm_gtim_chip *ls_pwm = dev_get_drvdata(dev);

	ls_pwm->ccer_reg = readl(ls_pwm->mmio_base + CTRL);
	ls_pwm->low_buffer_reg = readl(ls_pwm->mmio_base + LOW_BUFFER);
	ls_pwm->full_buffer_reg = readl(ls_pwm->mmio_base + FULL_BUFFER);
	return 0;
}

static int ls_pwm_gtim_resume(struct device *dev)
{
	struct ls_pwm_gtim_chip *ls_pwm = dev_get_drvdata(dev);

	writel(ls_pwm->ccer_reg, ls_pwm->mmio_base + CTRL);
	writel(ls_pwm->low_buffer_reg, ls_pwm->mmio_base + LOW_BUFFER);
	writel(ls_pwm->full_buffer_reg, ls_pwm->mmio_base + FULL_BUFFER);
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(ls_pwm_gtim_pm_ops, ls_pwm_gtim_suspend, ls_pwm_gtim_resume);

static const struct acpi_device_id loongson_pwm_acpi_match[] = {
	{"LOON0006"},
	{}};
MODULE_DEVICE_TABLE(acpi, loongson_pwm_acpi_match);

static struct platform_driver ls_pwm_gtim_driver = {
	.driver = {
		.name = "ls-pwm",
		.owner = THIS_MODULE,
		.bus = &platform_bus_type,
		.pm = &ls_pwm_gtim_pm_ops,
#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(ls_pwm_gtim_id_table),
#endif
		.acpi_match_table = ACPI_PTR(loongson_pwm_acpi_match),
	},
	.probe = ls_pwm_gtim_probe,
	.remove = ls_pwm_gtim_remove,
};
module_platform_driver(ls_pwm_gtim_driver);