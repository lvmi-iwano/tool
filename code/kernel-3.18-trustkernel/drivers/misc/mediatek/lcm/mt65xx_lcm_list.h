/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef __MT65XX_LCM_LIST_H__
#define __MT65XX_LCM_LIST_H__


extern LCM_DRIVER hct_r69339_dsi_vdo_hd_sharp_55_ln;
extern LCM_DRIVER hct_nt35521_dsi_vdo_hd_auo_50_rx;
extern LCM_DRIVER hct_otm1282a_dsi_vdo_hd_auo;
extern LCM_DRIVER hct_rm68200_dsi_vdo_hd_auo;
extern LCM_DRIVER hct_otm1901_dsi_vdo_fhd_auo;
extern LCM_DRIVER hct_nt35596_dsi_vdo_fhd_auo;
extern LCM_DRIVER hct_rm68200_dsi_vdo_hd_tm_50_ll;
extern LCM_DRIVER hct_rm67120_dsi_vdo_hd_auo_55_gx;
extern LCM_DRIVER hct_nt35532_dsi_vdo_fhd_auo_ips_55_xl;
extern LCM_DRIVER hct_nt35532_dsi_vdo_fhd_auo_55_ykl;
extern LCM_DRIVER hct_nt35532_dsi_vdo_fhd_auo_55_rx;
extern LCM_DRIVER hct_sh1386_dsi_vdo_hd_hx_55_hx;
extern LCM_DRIVER hct_r63350a_dsi_vdo_fhd_auo_55_hz;
extern LCM_DRIVER hct_nt35532_dsi_vdo_fhd_sharp_55_sh;
extern LCM_DRIVER hct_nt35596_dsi_vdo_fhd_auo_60_rx;
extern LCM_DRIVER hct_r63350_dsi_vdo_hd_auo_55_rx;

LCM_DRIVER *lcm_driver_list[] = {
#if defined(HCT_R69339_DSI_VDO_HD_SHARP_55_LN)
	&hct_r69339_dsi_vdo_hd_sharp_55_ln,
#endif
#if defined(HCT_NT35521_DSI_VDO_HD_AUO_50_RX)
	&hct_nt35521_dsi_vdo_hd_auo_50_rx,
#endif
#if defined(HCT_OTM1282A_DSI_VDO_HD_AUO)
	&hct_otm1282a_dsi_vdo_hd_auo,
#endif
#if defined(HCT_RM68200_DSI_VDO_HD_AUO)
	&hct_rm68200_dsi_vdo_hd_auo,
#endif
#if defined(HCT_OTM1901_DSI_VDO_FHD_AUO)
	&hct_otm1901_dsi_vdo_fhd_auo,
#endif
#if defined(HCT_NT35596_DSI_VDO_FHD_AUO)
	&hct_nt35596_dsi_vdo_fhd_auo,
#endif
#if defined(HCT_RM67120_DSI_VDO_HD_AUO_55_GX)
	&hct_rm67120_dsi_vdo_hd_auo_55_gx,
#endif
#if defined(HCT_RM68200_DSI_VDO_HD_TM_50_LL)
	&hct_rm68200_dsi_vdo_hd_tm_50_ll,
#endif
#if defined(HCT_NT35532_DSI_VDO_FHD_AUO_IPS_55_XL)
	&hct_nt35532_dsi_vdo_fhd_auo_ips_55_xl,
#endif
#if defined(HCT_NT35532_DSI_VDO_FHD_AUO_55_YKL)
	&hct_nt35532_dsi_vdo_fhd_auo_55_ykl,
#endif
#if defined(HCT_NT35532_DSI_VDO_FHD_SHARP_55_SH)
	&hct_nt35532_dsi_vdo_fhd_sharp_55_sh,
#endif
#if defined(HCT_NT35532_DSI_VDO_FHD_AUO_55_RX)
	&hct_nt35532_dsi_vdo_fhd_auo_55_rx,
#endif
#if defined(HCT_SH1386_DSI_VDO_HD_HX_55_HX)
	&hct_sh1386_dsi_vdo_hd_hx_55_hx,
#endif
#if defined(HCT_R63350A_DSI_VDO_FHD_AUO_55_HZ)
	&hct_r63350a_dsi_vdo_fhd_auo_55_hz,
#endif
#if defined(HCT_NT35596_DSI_VDO_FHD_AUO_60_RX)
	&hct_nt35596_dsi_vdo_fhd_auo_60_rx,
#endif
#if defined(HCT_R63350_DSI_VDO_HD_AUO_55_RX)
	&hct_r63350_dsi_vdo_hd_auo_55_rx,
#endif
};

#endif
