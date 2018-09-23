#
# Copyright (C) 2015 MediaTek Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#

#
# Makefile for misc devices that really don't fit anywhere else.
#

obj-y				+= kd_flashlightlist.o
obj-y				+= strobe_main_sid2_part1.o
obj-y				+= strobe_main_sid2_part2.o
obj-y				+= strobe_sub_sid2_part1.o
obj-y				+= strobe_sub_sid2_part2.o
obj-y				+= sub_strobe.o
obj-y				+= strobe_main_sid1_part2.o
obj-y				+= strobe_part_id.o
obj-y				+= strobe_sub_sid1_part2.o


ifeq ($(wildcard $(srctree)/drivers/misc/mediatek/mach/$(MTK_PLATFORM)/$(ARCH_MTK_PROJECT)/flashlight),)
#ifeq ($(wildcard $(srctree)/arch/arm/mach-$(MTK_PLATFORM)/$(ARCH_MTK_PROJECT)/flashlight),)

custom_kernel_flashlight := $(addsuffix /, $(shell echo $(CONFIG_CUSTOM_KERNEL_FLASHLIGHT)))

obj-y += $(custom_kernel_flashlight)

endif


HCT_FLASHLIGHT_NAME := $(subst ",,$(CONFIG_HCT_FLASHLIGHT))
ifneq ($(HCT_FLASHLIGHT_NAME),)
ifneq ($(wildcard $(srctree)/drivers/misc/mediatek/flashlight/src/mt6755/flashight_list/$(HCT_FLASHLIGHT_NAME).c),)
obj-y += flashight_list/$(HCT_FLASHLIGHT_NAME).o
else
$(error error: CONFIG_HCT_FLASHLIGHT: drivers/misc/mediatek/flashlight/src/mt6755/flashight_list/$(HCT_FLASHLIGHT_NAME).c is not exists!)
endif
endif

HCT_FLASHLIGHT_NAME := $(subst ",,$(CONFIG_HCT_FLASHLIGHT_SUB))
ifneq ($(HCT_FLASHLIGHT_NAME),)
ifneq ($(wildcard $(srctree)/drivers/misc/mediatek/flashlight/src/mt6755/flashight_list_sub/$(HCT_FLASHLIGHT_NAME).c),)
obj-y += flashight_list_sub/$(HCT_FLASHLIGHT_NAME).o
else
$(error error: CONFIG_HCT_FLASHLIGHT_SUB: drivers/misc/mediatek/flashlight/src/mt6755/flashight_list_sub/$(HCT_FLASHLIGHT_NAME).c is not exists!)
endif
endif
