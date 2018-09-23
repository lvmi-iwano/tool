#include <asm.S>

FUNC tsee_rtks_call , :
	b	tsee_main
END_FUNC tsee_rtks_call

FUNC tsee_rtks_ns_call , :
	b	tsee_ns_main
END_FUNC tsee_rtks_ns_call
