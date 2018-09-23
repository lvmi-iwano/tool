global-incdirs-y += .
srcs-y += dispatch.c
srcs-y += tsee_mm.c
srcs-$(CFG_TSEE_TEE) += tsee_sm.c
#srcs-y += tsee_rtks.c
srcs-y += tsee_rtks_call.S
srcs-y += tsee_fault.c



