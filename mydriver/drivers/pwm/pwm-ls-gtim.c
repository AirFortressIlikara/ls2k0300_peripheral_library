/*
 * @Author: ilikara 3435193369@qq.com
 * @Date: 2024-12-02 07:23:11
 * @LastEditors: ilikara 3435193369@qq.com
 * @LastEditTime: 2024-12-29 06:36:08
 * @FilePath: /ls2k0300_peripheral_library/mydriver/drivers/pwm/pwm-ls-gtim.c
 * @Description:
 *
 * Copyright (c) 2024 by ilikara 3435193369@qq.com, All Rights Reserved.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>

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
#define GTIM_CCMR(n) 0x18 + ((n) / 2) * 0x04
#define GTIM_CCER 0x20
#define GTIM_CNT 0x24
#define GTIM_PSC 0x28
#define GTIM_ARR 0x2C
#define GTIM_CCR(n) 0x34 + (n) * 0x04
#define GTIM_INSTA 0x50

/* CR1 each bit */
#define CR1_CEN BIT(0)

/* EGR each bit */
#define EGR_UG BIT(0)

/* CCMR each bit */
#define CCMR_CCnS(n) BIT((n) % 2 * 8 + 0)
#define CCMR_OCnFE(n) BIT((n) % 2 * 8 + 2)
#define CCMR_OCnPE(n) BIT((n) % 2 * 8 + 3)
#define CCMR_OCnM(n) BIT((n) % 2 * 8 + 4)
#define CCMR_OCnCE(n) BIT((n) % 2 * 8 + 7)

/* CCER each bit */
#define CCER_CCnE(n) BIT((n) * 4 + 0)
#define CCER_CCnP(n) BIT((n) * 4 + 1)

#define to_ls_pwm_gtim_chip(_chip) container_of(_chip, struct ls_pwm_gtim_chip, chip)
#define NS_IN_HZ (1000000000UL)
#define CPU_FRQ_PWM (50000000UL)

struct ls_pwm_gtim_chip
{
	struct pwm_chip chip;
	void __iomem *mmio_base;
	/* following registers used for suspend/resume */
	u32 arr_reg;
	u32 ccr_reg[4];
	u32 ccer_reg;
	u32 en_mark;
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
		val &= ~CCER_CCnP(pwm->hwpwm);
		break;
	case PWM_POLARITY_INVERSED:
		val |= CCER_CCnP(pwm->hwpwm);
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
	u32 cr1_reg, ccmr_reg;

	if (pwm->state.polarity == PWM_POLARITY_NORMAL)
		writel(ls_pwm->arr_reg, ls_pwm->mmio_base + GTIM_CCR(pwm->hwpwm));
	else if (pwm->state.polarity == PWM_POLARITY_INVERSED)
		writel(0, ls_pwm->mmio_base + GTIM_CCR(pwm->hwpwm));

	ccmr_reg = readl(ls_pwm->mmio_base + GTIM_CCMR(pwm->hwpwm));
	ccmr_reg &= ~(0b111 * CCMR_OCnM(pwm->hwpwm));
	writel(ccmr_reg, ls_pwm->mmio_base + GTIM_CCMR(pwm->hwpwm));

	ls_pwm->en_mark &= ~BIT(pwm->hwpwm);
	if (ls_pwm->en_mark == 0b0000)
	{
		cr1_reg = readl(ls_pwm->mmio_base + GTIM_CR1);
		cr1_reg &= ~CR1_CEN;
		writel(cr1_reg, ls_pwm->mmio_base + GTIM_CR1);
		writel(0x0, ls_pwm->mmio_base + GTIM_CNT);
	}
}

static int ls_pwm_gtim_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct ls_pwm_gtim_chip *ls_pwm = to_ls_pwm_gtim_chip(chip);
	int ret;
	u32 cr1_reg, ccmr_reg;

	writel(ls_pwm->ccr_reg[pwm->hwpwm], ls_pwm->mmio_base + GTIM_CCR(pwm->hwpwm));
	writel(ls_pwm->arr_reg, ls_pwm->mmio_base + GTIM_ARR);

	// todo:类似config函数中的特殊处理
	ccmr_reg = readl(ls_pwm->mmio_base + GTIM_CCMR(pwm->hwpwm));
	ccmr_reg |= 0b111 * CCMR_OCnM(pwm->hwpwm);
	writel(ccmr_reg, ls_pwm->mmio_base + GTIM_CCMR(pwm->hwpwm));

	ret = readl(ls_pwm->mmio_base + GTIM_CCER);
	ret |= CCER_CCnE(pwm->hwpwm);
	writel(ret, ls_pwm->mmio_base + GTIM_CCER);

	if (ls_pwm->en_mark == 0b0000)
	{
		writel(0x0, ls_pwm->mmio_base + GTIM_CNT);
		cr1_reg = readl(ls_pwm->mmio_base + GTIM_CR1);
		cr1_reg |= CR1_CEN;
		writel(cr1_reg, ls_pwm->mmio_base + GTIM_CR1);
	}
	ls_pwm->en_mark |= BIT(pwm->hwpwm);
	return 0;
}

static int ls_pwm_gtim_config(struct pwm_chip *chip, struct pwm_device *pwm,
							  int duty_ns, int period_ns)
{
	struct ls_pwm_gtim_chip *ls_pwm = to_ls_pwm_gtim_chip(chip);
	unsigned int arr_reg, ccr_reg;
	unsigned long long val0, val1;
	u32 ccmr_reg;

	if (period_ns > NS_IN_HZ || duty_ns > NS_IN_HZ)
		return -ERANGE;

	/*
	 * arr_reg = period_ns/(NSEC_PER_SEC/ls_pwm->clock_frequency);
	 * arr_reg = period_ns * ls_pwm->clock_frequency / NSEC_PER_SEC;
	 */

	if (duty_ns == 0)
	{
		ccmr_reg = readl(ls_pwm->mmio_base + GTIM_CCMR(pwm->hwpwm));
		ccmr_reg &= ~(0b111 * CCMR_OCnM(pwm->hwpwm));
		ccmr_reg |= 0b101 * CCMR_OCnM(pwm->hwpwm);
		writel(ccmr_reg, ls_pwm->mmio_base + GTIM_CCMR(pwm->hwpwm));
	}
	else if (period_ns == duty_ns)
	{
		ccmr_reg = readl(ls_pwm->mmio_base + GTIM_CCMR(pwm->hwpwm));
		ccmr_reg &= ~(0b111 * CCMR_OCnM(pwm->hwpwm));
		ccmr_reg |= 0b100 * CCMR_OCnM(pwm->hwpwm);
		writel(ccmr_reg, ls_pwm->mmio_base + GTIM_CCMR(pwm->hwpwm));
	}
	else
	{
		ccmr_reg = readl(ls_pwm->mmio_base + GTIM_CCMR(pwm->hwpwm));
		ccmr_reg |= 0b111 * CCMR_OCnM(pwm->hwpwm);
		writel(ccmr_reg, ls_pwm->mmio_base + GTIM_CCMR(pwm->hwpwm));
	}

	val0 = ls_pwm->clock_frequency * period_ns;
	do_div(val0, NSEC_PER_SEC);
	arr_reg = val0 < 1 ? 1 : val0;

	val1 = ls_pwm->clock_frequency * duty_ns;
	do_div(val1, NSEC_PER_SEC);
	ccr_reg = val1 < 1 ? 1 : val1;

	writel(ccr_reg, ls_pwm->mmio_base + GTIM_CCR(pwm->hwpwm));
	writel(arr_reg, ls_pwm->mmio_base + GTIM_ARR);
	writel(0x0, ls_pwm->mmio_base + GTIM_CNT);

	ls_pwm->arr_reg = arr_reg;
	ls_pwm->ccr_reg[pwm->hwpwm] = ccr_reg;
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
	unsigned int arr_reg, ccr_reg;
	u32 ccer_reg;

	/*
	 * period_ns = arr_reg *NSEC_PER_SEC /ls_pwm->clock_frequency.
	 */
	arr_reg = readl(ls_pwm->mmio_base + GTIM_ARR);
	state->period = ls_pwm_gtim_reg_to_ns(ls_pwm, arr_reg);

	ccr_reg = readl(ls_pwm->mmio_base + GTIM_CCR(pwm->hwpwm));
	state->duty_cycle = ls_pwm_gtim_reg_to_ns(ls_pwm, ccr_reg);

	ccer_reg = readl(ls_pwm->mmio_base + GTIM_CCER);
	state->polarity = (ccer_reg & CCER_CCnP(pwm->hwpwm)) ? PWM_POLARITY_INVERSED
														 : PWM_POLARITY_NORMAL;
	state->enabled = (ccer_reg & CCER_CCnE(pwm->hwpwm)) ? true : false;

	if (state->enabled)
	{
		ls_pwm->en_mark |= BIT(pwm->hwpwm);
	}
	else
	{
		ls_pwm->en_mark &= ~BIT(pwm->hwpwm);
	}

	ls_pwm->ccer_reg = ccer_reg;
	ls_pwm->ccr_reg[pwm->hwpwm] = readl(ls_pwm->mmio_base + GTIM_CCR(pwm->hwpwm));
	ls_pwm->arr_reg = readl(ls_pwm->mmio_base + GTIM_ARR);
}

static const struct pwm_ops ls_pwm_gtim_ops = {
	.config = ls_pwm_gtim_config,
	.set_polarity = ls_pwm_gtim_set_polarity,
	.enable = ls_pwm_gtim_enable,
	.disable = ls_pwm_gtim_disable,
	.get_state = ls_pwm_gtim_get_state,
	.owner = THIS_MODULE,
};

static int ls_pwm_gtim_probe(struct platform_device *pdev)
{
	struct ls_pwm_gtim_chip *pwm;
	struct resource *mem;
	int err;
	struct device_node *np = pdev->dev.of_node;
	u32 clk;

	pwm = devm_kzalloc(&pdev->dev, sizeof(*pwm), GFP_KERNEL);
	if (!pwm)
	{
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}

	pwm->en_mark = 0b0000;

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

	err = pwmchip_add(&pwm->chip);
	if (err < 0)
	{
		dev_err(&pdev->dev, "pwmchip_add() failed: %d\n", err);
		return err;
	}

	writel(EGR_UG, pwm->mmio_base + GTIM_EGR);

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
	err = pwmchip_remove(&pwm->chip);
	if (err < 0)
		return err;

	return 0;
}

#ifdef CONFIG_OF
static struct of_device_id ls_pwm_gtim_id_table[] = {
	{.compatible = "loongson,ls300-pwm-gtim"},
	{},
};
MODULE_DEVICE_TABLE(of, ls_pwm_gtim_id_table);
#endif

#ifdef CONFIG_PM_SLEEP
static int ls_pwm_gtim_suspend(struct device *dev)
{
	struct ls_pwm_gtim_chip *ls_pwm = dev_get_drvdata(dev);

	ls_pwm->ccer_reg = readl(ls_pwm->mmio_base + GTIM_CCER);
	ls_pwm->ccr_reg[0] = readl(ls_pwm->mmio_base + GTIM_CCR(0));
	ls_pwm->ccr_reg[1] = readl(ls_pwm->mmio_base + GTIM_CCR(1));
	ls_pwm->ccr_reg[2] = readl(ls_pwm->mmio_base + GTIM_CCR(2));
	ls_pwm->ccr_reg[3] = readl(ls_pwm->mmio_base + GTIM_CCR(3));
	ls_pwm->arr_reg = readl(ls_pwm->mmio_base + GTIM_ARR);
	return 0;
}

static int ls_pwm_gtim_resume(struct device *dev)
{
	struct ls_pwm_gtim_chip *ls_pwm = dev_get_drvdata(dev);

	writel(ls_pwm->ccer_reg, ls_pwm->mmio_base + GTIM_CCER);
	writel(ls_pwm->ccr_reg[0], ls_pwm->mmio_base + GTIM_CCR(0));
	writel(ls_pwm->ccr_reg[1], ls_pwm->mmio_base + GTIM_CCR(1));
	writel(ls_pwm->ccr_reg[2], ls_pwm->mmio_base + GTIM_CCR(2));
	writel(ls_pwm->ccr_reg[3], ls_pwm->mmio_base + GTIM_CCR(3));
	writel(ls_pwm->arr_reg, ls_pwm->mmio_base + GTIM_ARR);
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(ls_pwm_gtim_pm_ops, ls_pwm_gtim_suspend, ls_pwm_gtim_resume);

static struct platform_driver ls_pwm_gtim_driver = {
	.driver = {
		.name = "ls-pwm-gtim",
		.owner = THIS_MODULE,
		.bus = &platform_bus_type,
		.pm = &ls_pwm_gtim_pm_ops,
#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(ls_pwm_gtim_id_table),
#endif
	},
	.probe = ls_pwm_gtim_probe,
	.remove = ls_pwm_gtim_remove,
};
module_platform_driver(ls_pwm_gtim_driver);

MODULE_AUTHOR("Ilikara <3435193369@qq.com>");
MODULE_DESCRIPTION("Loongson Gtimer Pwm Driver");
MODULE_LICENSE("GPL");