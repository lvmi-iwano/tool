#include <linux/tsee/thee.h>

#define thee_ELF_PATH "../../../../../kernel-3.18/init/tsee/src/thee.elf"


.global _thee_goto_EL2
_thee_goto_EL2:
	smc #0
	isb
	ret

.section .thee, "ax"
.global _sthee
_sthee:
.incbin thee_ELF_PATH
.global _ethee
_ethee:
.section .text
