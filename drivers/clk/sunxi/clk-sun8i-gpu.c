/*
 * Copyright 2016 BLOKS
 * Copyright 2016 Free Electrons
 *
 * Maxime Ripard <maxime.ripard@free-electrons.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/clk-provider.h>
#include <linux/of_address.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#define SUN8I_A23_GPU_GATE_BIT	31
#define SUN8I_A23_GPU_DIV_WIDTH	2
#define SUN8I_A23_GPU_DIV_SHIFT	0

static DEFINE_SPINLOCK(sun8i_a23_gpu_lock);

static void __init sun8i_a23_gpu_setup(struct device_node *node)
{
	const char *parent;
	const char *clk_name = node->name;
	struct clk_divider *div;
	struct clk_gate *gate;
	struct resource res;
	void __iomem *reg;
	struct clk *clk;

	of_property_read_string(node, "clock-output-names", &clk_name);

	reg = of_io_request_and_map(node, 0, of_node_full_name(node));
	if (IS_ERR(reg)) {
		pr_err("%s: Could not map the clock registers\n", clk_name);
		return;
	}

	parent = of_clk_get_parent_name(node, 0);

	gate = kzalloc(sizeof(*gate), GFP_KERNEL);
	if (!gate)
		goto unmap;

	gate->reg = reg;
	gate->bit_idx = SUN8I_A23_GPU_GATE_BIT;
	gate->lock = &sun8i_a23_gpu_lock;

	div = kzalloc(sizeof(*div), GFP_KERNEL);
	if (!div)
		goto free_gate;

	div->reg = reg;
	div->shift = SUN8I_A23_GPU_DIV_SHIFT;
	div->width = SUN8I_A23_GPU_DIV_WIDTH;
	div->lock = &sun8i_a23_gpu_lock;

	clk = clk_register_composite(NULL, clk_name,
				     &parent, 1,
				     NULL, NULL,
				     &div->hw, &clk_divider_ops,
				     &gate->hw, &clk_gate_ops,
				     CLK_SET_RATE_PARENT);
	if (IS_ERR(clk)) {
		pr_err("%s: Couldn't register the clock\n", clk_name);
		goto free_div;
	}

	of_clk_add_provider(node, of_clk_src_simple_get, clk);

	return;

free_div:
	kfree(div);
free_gate:
	kfree(gate);
unmap:
	iounmap(reg);
	of_address_to_resource(node, 0, &res);
	release_mem_region(res.start, resource_size(&res));
}

CLK_OF_DECLARE(sun8i_a23_gpu, "allwinner,sun8i-a23-gpu-clk",
	       sun8i_a23_gpu_setup);
