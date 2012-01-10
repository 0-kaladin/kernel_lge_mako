/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/err.h>
#include <mach/rpm.h>

#include "rpm_resources.h"
#include "qdss.h"

enum {
	QDSS_CLK_OFF,
	QDSS_CLK_ON_DBG,
	QDSS_CLK_ON_HSDBG,
};


int qdss_clk_enable(void)
{
	int ret;

	struct msm_rpm_iv_pair iv;
	iv.id = MSM_RPM_ID_QDSS_CLK;
	iv.value = QDSS_CLK_ON_DBG;
	ret = msm_rpmrs_set(MSM_RPM_CTX_SET_0, &iv, 1);
	if (WARN(ret, "qdss clks not enabled (%d)\n", ret))
		goto err_clk;

	return 0;

err_clk:
	return ret;
}

void qdss_clk_disable(void)
{
	int ret;
	struct msm_rpm_iv_pair iv;

	iv.id = MSM_RPM_ID_QDSS_CLK;
	iv.value = QDSS_CLK_OFF;
	ret = msm_rpmrs_set(MSM_RPM_CTX_SET_0, &iv, 1);
	WARN(ret, "qdss clks not disabled (%d)\n", ret);
}
