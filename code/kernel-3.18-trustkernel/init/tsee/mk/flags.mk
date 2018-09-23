# 32-bit flags 
arm32-cflags   += -march=armv7-a -marm
arm32-cflags	+= -pipe -mthumb-interwork -mlong-calls
arm32-cflags	+= -fno-short-enums -mno-apcs-float -fno-common

arm32-cflags	+= -mfloat-abi=soft

arm32-cflags	+= -mno-unaligned-access
arm32-aflags	+= -march=armv7-a 

# 64-bit flags
arm64-cflags	+= -mgeneral-regs-only
arm64-cflags	+= -mstrict-align


ifeq ($(CFG_TSEE_TEE), y)
TSEE_BUILD_EXTRA_FLAGS += -DCFG_TSEE_TEE
else
TSEE_BUILD_EXTRA_FLAGS += -DCFG_TSEE_LINUX
endif

TSEE_BUILD_EXTRA_FLAGS += -DCONFIG_TRUSTKERNEL_TSEE_RTKS

PLATFORM_FLAVOR ?= default
platform_flavor_$(PLATFORM_FLAVOR) := y
cppflags   += -DPLATFORM_FLAVOR=PLATFORM_FLAVOR_ID_$(PLATFORM_FLAVOR) 

cppflags   += -include $(out-dir)/include/generated/conf.h
cppflags   += $(platform-cppflags) $(TSEE_BUILD_EXTRA_FLAGS)
cflags 	   += $(platform-cflags) $(TSEE_BUILD_EXTRA_FLAGS)
aflags 	   += $(platform-aflags) $(TSEE_BUILD_EXTRA_FLAGS)

cppflags   += -Iinclude

