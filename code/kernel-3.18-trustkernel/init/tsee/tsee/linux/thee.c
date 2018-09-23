/*
 *
 * Copyright (C) 2015 - 2017 TrustKernel Team, Shanghai Pingbo Information Technology Corporation Limited - All Rights Reserved
 *
 *      https://www.trustkernel.com
 *
 * This file is part of T6 and TSEE.
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <asm/cacheflush.h>
#include <asm/irqflags.h>
#include <linux/fs.h>
#include <asm/tlbflush.h>
#include <linux/init.h>
#include <asm/io.h>


#include <linux/tsee/thee.h>
#include "ld.h"

#define THEE_32BIT_SMC_CALL_MAGIC 0x82000400
#define THEE_64BIT_SMC_CALL_MAGIC 0xC2000400

#define THEE_STACK_OFFSET 4096

#define THEE_MODE_AARCH32 0
#define THEE_MODE_AARCH64 1

extern char _sthee;
extern char _ethee;

char *thee;
size_t thee_size;

int thee_entry(void);

int _thee_goto_EL2(int magic, void *label, int offset, int mode, void *base, int size);

int thee_resolve(void *binary, _Elf_Sym *sym, _Elf_Sxword *value) {

	char *name;
	char *strtab;
	size_t strtabsz;

	printk(KERN_ALERT "%s\n", __FUNCTION__);

	if(ld_get_dynamic_strtab(binary, &strtab, &strtabsz)) { return -1; }

	if(ld_get_string(strtab, sym->st_name, &name)) { return -1; }

	printk(KERN_ALERT "name: %s\n", name);

	return -1;
}

int thee_translate(void *binary, void *in, void **out) {

	printk(KERN_ALERT "%s\n", __FUNCTION__);

	*out = (void *)virt_to_phys(in);

	return 0;
}

int thee_entry(void) {

	typedef int (*_main_)(void);

	int status;
	_main_ entry_point;

	printk(KERN_ALERT "%s\n", __FUNCTION__);

	if(ld_get_entry(thee, (void **)&entry_point)) { return -1; }

	entry_point = (_main_)virt_to_phys(entry_point);

	printk(KERN_ALERT "thee entry point: %p\n", entry_point);

	thee = (char *)virt_to_phys(thee);

	flush_cache_all();

	status = _thee_goto_EL2(THEE_64BIT_SMC_CALL_MAGIC, entry_point, THEE_STACK_OFFSET, THEE_MODE_AARCH64, thee, thee_size);

	printk(KERN_ALERT "thee(%p, 0x%x): %x\n", thee, (int)thee_size, status);

	return 0;
}

int thee_init(void) {

	size_t size;
	char *name;
	void *base;

	printk(KERN_ALERT "%s\n", __FUNCTION__);

	if(smp_processor_id() != 0) { return 0; }

	printk(KERN_ALERT "bin 0x%p, 0x%x\n", &_sthee, (int)(&_ethee - &_sthee));

	memcpy((void *)phys_to_virt(THEE_RUNTIME_BASE),  &_sthee, (size_t)(&_ethee - &_sthee));

	thee = (void *)phys_to_virt(THEE_RUNTIME_BASE);
	thee_size = THEE_RUNTIME_SIZE;

	printk(KERN_ALERT "ram 0x%p, 0x%x\n", thee, (int)thee_size);

	if(ld_get_size(thee, &size)) { return -1; }

	if(ld_get_name(thee, &name)) { return -1; }

	printk(KERN_ALERT "%s, %d\n", name, (int)size);

	if(ld_fixup_dynamic_relatab(thee, &thee_resolve, &thee_translate)) { return -1; }

	if(ld_fixup_dynamic_plttab(thee, &thee_resolve, &thee_translate)) { return -1; }

	if(ld_get_sect(thee, ".bss", &base, &size)) { return -1; }

	memset(base, 0, size);

	return thee_entry();
}
