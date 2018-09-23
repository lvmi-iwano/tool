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


#include <linux/tsee/kee.h>
#include "ld.h"

#if 0

#define KEE_STACK_OFFSET 4096

#define KEE_MODE_AARCH32 0
#define KEE_MODE_AARCH64 1

extern char _skee;
extern char _ekee;

char *kee;
size_t kee_size;

int kee_entry(void);

int _kee_goto_EL2(int magic, void *label, int offset, int mode, void *base, int size);

int kee_resolve(void *binary, _Elf_Sym *sym, _Elf_Sxword *value) {

	char *name;
	char *strtab;
	size_t strtabsz;

	printk(KERN_ALERT "%s\n", __FUNCTION__);

	if(ld_get_dynamic_strtab(binary, &strtab, &strtabsz)) { return -1; }

	if(ld_get_string(strtab, sym->st_name, &name)) { return -1; }

	printk(KERN_ALERT "name: %s\n", name);

	return -1;
}

int kee_translate(void *binary, void *in, void **out) {

	printk(KERN_ALERT "%s\n", __FUNCTION__);

	*out = (void *)virt_to_phys(in);

	return 0;
}

int kee_entry(void) {

	typedef int (*_main_)(void);

	int status;
	_main_ entry_point;

	printk(KERN_ALERT "%s\n", __FUNCTION__);

	if(ld_get_entry(kee, (void **)&entry_point)) { return -1; }

	entry_point = (_main_)virt_to_phys(entry_point);

	printk(KERN_ALERT "kee entry point: %p\n", entry_point);

	kee = (char *)virt_to_phys(kee);

	flush_cache_all();

	status = _kee_entry(entry_point, KEE_STACK_OFFSET, KEE_MODE_AARCH64, kee, kee_size);

	printk(KERN_ALERT "kee(%p, 0x%x): %x\n", kee, (int)kee_size, status);

	return 0;
}

int kee_init(void) {

	size_t size;
	char *name;
	void *base;

	printk(KERN_ALERT "%s\n", __FUNCTION__);

	if(smp_processor_id() != 0) { return 0; }

	printk(KERN_ALERT "bin 0x%p, 0x%x\n", &_skee, (int)(&_ekee - &_skee));

	memcpy((void *)phys_to_virt(KEE_RUNTIME_BASE),  &_skee, (size_t)(&_ekee - &_skee));

	kee = (void *)phys_to_virt(KEE_RUNTIME_BASE);
	kee_size = KEE_RUNTIME_SIZE;

	printk(KERN_ALERT "ram 0x%p, 0x%x\n", kee, (int)kee_size);

	if(ld_get_size(kee, &size)) { return -1; }

	if(ld_get_name(kee, &name)) { return -1; }

	printk(KERN_ALERT "%s, %d\n", name, (int)size);

	if(ld_fixup_dynamic_relatab(kee, &kee_resolve, &kee_translate)) { return -1; }

	if(ld_fixup_dynamic_plttab(kee, &kee_resolve, &kee_translate)) { return -1; }

	if(ld_get_sect(kee, ".bss", &base, &size)) { return -1; }

	memset(base, 0, size);

	return kee_entry();
}
#endif 


int kee_init(void) {
	return 0; 
}
