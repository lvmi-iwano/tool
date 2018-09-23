#ifdef CFG_TSEE_LINUX
#include <linux/linkage.h>
#include <linux/tsee/tsee_entry.h>

#ifdef CONFIG_TRUSTKERNEL_TSEE_RTKS_VMM

ENTRY(tsee_rtks_call)
	hvc #0
	ret
ENDPROC(tsee_rtks_call)

#elif defined(CONFIG_TRUSTKERNEL_TSEE_RTKS_KEE)

ENTRY(tsee_rtks_call)
	b	tsee_main
ENDPROC(tsee_rtks_call)


#endif

#else 

#include <asm.S>

FUNC tsee_rtks_call , :
	b	tsee_main
END_FUNC tsee_rtks_call

FUNC tsee_rtks_ns_call , :
	b	tsee_ns_main
END_FUNC tsee_rtks_ns_call



#endif
