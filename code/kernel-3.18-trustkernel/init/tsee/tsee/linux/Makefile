ccflags-y += -DCFG_TSEE_LINUX
asflags-y += -DCFG_TSEE_LINUX

ifneq ($(strip $(M)),)
	obj-m += tsee.o
else
	obj-y += tsee.o
endif

M ?= $(srctree)/init/tsee

ccflags-y+=-I$(M)/src

tsee-objs := \
	ld.o \
	tsee_rtks.o \
	tsee_rtks_call.o \
	tsee_mm.o \


ifeq ($(CONFIG_TRUSTKERNEL_TSEE_RTKS),y)
	ccflags-y += -DCONFIG_TRUSTKERNEL_TSEE_RTKS

	tsee-objs += \


ifeq ($(CONFIG_TRUSTKERNEL_TSEE_RTKS_TZ),y)
	ccflags-y += -DCONFIG_TRUSTKERNEL_TSEE_RTKS_TZ
	tsee-objs += \

endif
ifeq ($(CONFIG_TRUSTKERNEL_TSEE_RTKS_VMM),y)
	ccflags-y += -DCONFIG_TRUSTKERNEL_TSEE_RTKS_VMM
	tsee-objs += \
		thee.o \
		_thee.o \

endif
ifeq ($(CONFIG_TRUSTKERNEL_TSEE_RTKS_KEE),y)
	ccflags-y += -DCONFIG_TRUSTKERNEL_TSEE_RTKS_KEE
	tsee-objs += \
		kee.o \
		_kee.o \
		dispatch.o \

endif



endif



