# Default configuration values for TrustKernel TSEE.

ifeq ($(TARGET_PLAT),)
$(error TARGET_PLAT not defined)
endif

# Cross-compiler prefix and suffix
CROSS_COMPILE := arm-linux-gnueabihf-
COMPILER := gcc

# 64-bit 32-bit flags, currently we only support 32-bit
ARCH := arm
#ARCH := arm64

# Compiler warning level.
# Supported values: undefined, 1, 2 and 3. 3 gives more warnings.
WARNS ?= 3

TSEE_BUILD := ${TKCORE_BUILD}

# use RELEASE if TSEE_BUILD not defined
ifeq ($(TSEE_BUILD),)
ifneq ($(RELEASE),)
TSEE_BUILD := release
else
TSEE_BUILD := debug
endif
endif

ifeq ($(TSEE_BUILD), release)
TSEE_MODIFIER := "rls"
else
ifeq ($(TSEE_BUILD), debug)
TSEE_MODIFIER := "dbg"
else
$(error BAD TSEE_BUILD=$(TKCORE_BUILD))
endif
endif

ifeq ($(TSEE_TARGET),)
CFG_TSEE_TEE := y
CFG_TSEE_LINUX := n
else 
ifneq ($(TSEE_TARGET), linux)
CFG_TSEE_TEE := y
CFG_TSEE_LINUX := n
else 
CFG_TSEE_TEE := n
CFG_TSEE_LINUX := y
endif
endif

# Implementation description 
CFG_TSEE_IMPL_DESCR ?= TrustKernel_TSEE
CFG_TSEE_IMPL_VERSION ?= $(shell git describe --always --dirty=-dev 2>/dev/null || echo Unknown)--$(TSEE_MODIFIER)
CFG_TSEE_MANUFACTURER ?= TrustKernel

