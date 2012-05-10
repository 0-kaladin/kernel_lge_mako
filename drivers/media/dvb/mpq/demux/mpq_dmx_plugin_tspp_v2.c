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

#include <linux/init.h>
#include <linux/module.h>
#include "mpq_dvb_debug.h"
#include "mpq_dmx_plugin_common.h"


static int mpq_tspp_dmx_start_filtering(struct dvb_demux_feed *feed)
{
	MPQ_DVB_DBG_PRINT(
		"%s(%d) executed\n",
		__func__,
		feed->pid);

	/* Always feed sections/PES starting from a new one and
	 * do not partial transfer data from older one
	 */
	feed->pusi_seen = 0;
	return 0;
}

static int mpq_tspp_dmx_stop_filtering(struct dvb_demux_feed *feed)
{
	MPQ_DVB_DBG_PRINT(
		"%s(%d) executed\n",
		__func__,
		feed->pid);

	return 0;
}

/**
 * Initialize a single demux device.
 *
 * @mpq_adapter: MPQ DVB adapter
 * @mpq_demux: The demux device to initialize
 *
 * Return     error code
 */
static int mpq_tspp_dmx_init(
				struct dvb_adapter *mpq_adapter,
				struct mpq_demux *mpq_demux)
{
	int result;

	MPQ_DVB_DBG_PRINT("%s executed\n", __func__);

	/* Set the kernel-demux object capabilities */
	mpq_demux->demux.dmx.capabilities =
		DMX_TS_FILTERING			|
		DMX_PES_FILTERING			|
		DMX_SECTION_FILTERING		|
		DMX_MEMORY_BASED_FILTERING	|
		DMX_CRC_CHECKING			|
		DMX_TS_DESCRAMBLING;

	/* Set dvb-demux "virtual" function pointers */
	mpq_demux->demux.priv = (void *)mpq_demux;
	mpq_demux->demux.filternum = MPQ_DMX_MAX_NUM_OF_FILTERS;
	mpq_demux->demux.feednum = MPQ_DMX_MAX_NUM_OF_FILTERS;
	mpq_demux->demux.start_feed = mpq_tspp_dmx_start_filtering;
	mpq_demux->demux.stop_feed = mpq_tspp_dmx_stop_filtering;
	mpq_demux->demux.write_to_decoder = NULL;
	mpq_demux->demux.decoder_fullness_init = NULL;
	mpq_demux->demux.decoder_fullness_wait = NULL;
	mpq_demux->demux.decoder_fullness_abort = NULL;

	/* Initialize dvb_demux object */
	result = dvb_dmx_init(&mpq_demux->demux);
	if (result < 0) {
		MPQ_ERR_PRINT("%s: dvb_dmx_init failed\n", __func__);
		goto init_failed;
	}

	/* Now initailize the dmx-dev object */
	mpq_demux->dmxdev.filternum = MPQ_DMX_MAX_NUM_OF_FILTERS;
	mpq_demux->dmxdev.demux = &mpq_demux->demux.dmx;
	mpq_demux->dmxdev.capabilities =
		DMXDEV_CAP_DUPLEX |
		DMXDEV_CAP_PULL_MODE |
		DMXDEV_CAP_PCR_EXTRACTION;

	mpq_demux->dmxdev.demux->set_source = mpq_dmx_set_source;

	result = dvb_dmxdev_init(&mpq_demux->dmxdev, mpq_adapter);
	if (result < 0) {
		MPQ_DVB_ERR_PRINT("%s: dvb_dmxdev_init failed (errno=%d)\n",
						  __func__,
						  result);

		goto init_failed_dmx_release;
	}

	return 0;

init_failed_dmx_release:
	dvb_dmx_release(&mpq_demux->demux);
init_failed:
	return result;
}

static int __init mpq_dmx_tspp_plugin_init(void)
{
	MPQ_DVB_DBG_PRINT("%s executed\n", __func__);

	return mpq_dmx_plugin_init(mpq_tspp_dmx_init);
}

static void __exit mpq_dmx_tspp_plugin_exit(void)
{
	MPQ_DVB_DBG_PRINT("%s executed\n", __func__);
	mpq_dmx_plugin_exit();
}


module_init(mpq_dmx_tspp_plugin_init);
module_exit(mpq_dmx_tspp_plugin_exit);

MODULE_DESCRIPTION("Qualcomm demux TSPP version2 HW Plugin");
MODULE_LICENSE("GPL v2");

