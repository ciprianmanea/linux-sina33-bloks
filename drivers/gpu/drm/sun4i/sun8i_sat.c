/*
 * Copyright (C) 2016 Free Electrons
 *
 * Maxime Ripard <maxime.ripard@free-electrons.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#include <linux/clk.h>
#include <linux/component.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/reset.h>

struct sun8i_sat {
	struct clk		*clk;
	struct reset_control	*reset;
};

static int sun8i_sat_bind(struct device *dev, struct device *master,
			 void *data)
{
	struct sun8i_sat *sat;
	int ret;

	sat = devm_kzalloc(dev, sizeof(*sat), GFP_KERNEL);
	if (!sat)
		return -ENOMEM;
	dev_set_drvdata(dev, sat);

	sat->reset = devm_reset_control_get(dev, NULL);
	if (IS_ERR(sat->reset)) {
		dev_err(dev, "Couldn't get our reset line\n");
		return PTR_ERR(sat->reset);
	}

	ret = reset_control_deassert(sat->reset);
	if (ret) {
		dev_err(dev, "Couldn't deassert our reset line\n");
		return ret;
	}

	sat->clk = devm_clk_get(dev, NULL);
	if (IS_ERR(sat->clk)) {
		dev_err(dev, "Couldn't get our clock clock\n");
		ret = PTR_ERR(sat->clk);
		goto err_assert_reset;
	}

	return clk_prepare_enable(sat->clk);

err_assert_reset:
	reset_control_assert(sat->reset);
	return ret;
}

static void sun8i_sat_unbind(struct device *dev, struct device *master,
			    void *data)
{
	struct sun8i_sat *sat = dev_get_drvdata(dev);

	clk_disable_unprepare(sat->clk);
	reset_control_assert(sat->reset);
}

static struct component_ops sun8i_sat_ops = {
	.bind	= sun8i_sat_bind,
	.unbind	= sun8i_sat_unbind,
};

static int sun8i_sat_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &sun8i_sat_ops);
}

static int sun8i_sat_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &sun8i_sat_ops);

	return 0;
}

static const struct of_device_id sun8i_sat_of_table[] = {
	{ .compatible = "allwinner,sun8i-a33-sat" },
	{ }
};
MODULE_DEVICE_TABLE(of, sun8i_sat_of_table);

static struct platform_driver sun8i_sat_platform_driver = {
	.probe		= sun8i_sat_probe,
	.remove		= sun8i_sat_remove,
	.driver		= {
		.name		= "sun8i-sat",
		.of_match_table	= sun8i_sat_of_table,
	},
};
module_platform_driver(sun8i_sat_platform_driver);

MODULE_AUTHOR("Maxime Ripard <maxime.ripard@free-electrons.com>");
MODULE_DESCRIPTION("Allwinner A33 Saturation Enhancement (SAT) Driver");
MODULE_LICENSE("GPL");
