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

#ifndef __LD_H__
#define __LD_H__


#define __TARGET_64__

#include "elf.h"

typedef int (*ld_init_t)(void);
typedef int (*ld_fini_t)(void);

#ifdef __TARGET_64__
typedef int (*ld_resolve_t)(void *binary, _Elf_Sym *sym, _Elf_Sxword *value);
#else //__TARGET_32__
typedef int (*ld_resolve_t)(void *binary, _Elf_Sym *sym, _Elf_Sword *value);
#endif //__TARGET_64__ | __TARGET_32__

typedef int (*ld_translate_t)(void *binary, void *in, void **out);

int ld_Elf_Ehdr_to_Elf_Shdr(_Elf_Ehdr *ehdr, _Elf_Shdr **shdr, size_t *size);

int ld_Elf_Ehdr_to_Elf_Phdr(_Elf_Ehdr *ehdr, _Elf_Phdr **phdr, size_t *size);

int ld_binary_to_Elf_Ehdr(void *binary, _Elf_Ehdr **ehdr);

int ld_get_entry(void *binary, void **entry);

int ld_get_name(void *binary, char **name);

int ld_get_version(void *binary, char **version);

int ld_get_string(char *strtab, int index, char **string);

int ld_get_symbol(_Elf_Sym *symtab, int index, _Elf_Sym **symbol);

int ld_get_base(void *binary, void **address);

int ld_get_size(void *binary, size_t *size);

int ld_get_sect(void *binary, char *name, void **section, size_t *size);

#ifdef __TARGET_64__
int ld_get_dynamic_relatab(void *binary, _Elf_Rela **rela, size_t *size);
#else //__TARGET_32__
int ld_get_dynamic_reltab(void *binary, _Elf_Rel **rel, size_t *size);
#endif //__TARGET_64__ | __TARGET_32__

int ld_get_dynamic_symtab(void *binary, _Elf_Sym **symtab, size_t *size);

int ld_get_dynamic_strtab(void *binary, char **strtab, size_t *size);

#ifdef __TARGET_64__
int ld_fixup_dynamic_relatab(void *binary, ld_resolve_t resolve, ld_translate_t translate);
#else //__TARGET_32__
int ld_fixup_dynamic_reltab(void *binary, ld_resolve_t resolve, ld_translate_t translate);
#endif //__TARGET_64__ | __TARGET_32__

int ld_fixup_dynamic_plttab(void *binary, ld_resolve_t resolve, ld_translate_t translate);

#endif //__LD_H__
