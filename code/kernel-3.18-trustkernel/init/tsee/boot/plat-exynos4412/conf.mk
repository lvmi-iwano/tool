include boot/plat-$(TARGET_PLAT)/platform_flags.mk

#CROSS_COMPILE	:= arm-linux-gnueabihf-
CROSS_COMPILE	:= ${CROSS_COMPILE_NEWLIB}
COMPILER	?= gcc


platform-cppflags	+= -I$(platform-dir)

platform-subdirs += \
	$(platform-dir)

CFG_EXYNOS4412_UART := y
CFG_GIC := y
CFG_EXYNOS := y

