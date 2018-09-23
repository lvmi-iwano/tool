PLATFORM_FLAVOR ?= exynos4412
PLATFORM_FLAVOR_$(PLATFORM_FLAVOR) := y

platform-cpuarch	:= cortex-a9
platform-cflags	+= -mcpu=$(platform-cpuarch) 
platform-cflags	+= -pipe -mthumb-interwork -mlong-calls
platform-cflags	+= -fno-short-enums -mno-apcs-float -fno-common
platform-cflags	+= -mfloat-abi=soft
#platform-cflags	+= -mfloat-abi=softfp
platform-aflags	+= -mcpu=$(platform-cpuarch)
#platform-aflags += -mfpu=neon

platform-cflags += -ffunction-sections -fdata-sections

DEBUG		?= 1
ifeq ($(DEBUG),1)
#platform-cflags += -O0
latform-cflags += -Os
else
platform-cflags += -Os
endif

platform-cflags += -g
platform-aflags += -g

