#include <platform_config.h>

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)

ENTRY(_start)
SECTIONS
{
	. = CFG_TSEE_LOAD_ADDR;

	/* text/read-only data */
	.text : {
		__text_start = .;
		KEEP(*(.text.boot.vectab1))
		KEEP(*(.text.boot.vectab2))
		KEEP(*(.text.boot))

		. = ALIGN(0x1000);
		__tsee_text_ro_start = .;
		KEEP(*(.text.tsee_ro))
		. = ALIGN(0x1000);
		__tsee_text_ro_end = .;

		*(.text .text.*)
		*(.sram.text.glue_7* .gnu.linkonce.t.*)
		__text_end = .;
	}

	.interp : { *(.interp) }
	.dynsym : { *(.dynsym) }
	.dynstr : { *(.dynstr) }
	.rel.text : { *(.rel.text) *(.rel.gnu.linkonce.t*) }
	.rela.text : { *(.rela.text) *(.rela.gnu.linkonce.t*) }
	.rel.data : { *(.rel.data) *(.rel.gnu.linkonce.d*) }
	.rela.data : { *(.rela.data) *(.rela.gnu.linkonce.d*) }
	.rel.rodata : { *(.rel.rodata) *(.rel.gnu.linkonce.r*) }
	.rela.rodata : { *(.rela.rodata) *(.rela.gnu.linkonce.r*) }
	.rel.got : { *(.rel.got) }
	.rela.got : { *(.rela.got) }
	.rel.ctors : { *(.rel.ctors) }
	.rela.ctors : { *(.rela.ctors) }
	.rel.dtors : { *(.rel.dtors) }
	.rela.dtors : { *(.rela.dtors) }
	.rel.init : { *(.rel.init) }
	.rela.init : { *(.rela.init) }
	.rel.fini : { *(.rel.fini) }
	.rela.fini : { *(.rela.fini) }
	.rel.bss : { *(.rel.bss) }
	.rela.bss : { *(.rela.bss) }
	.rel.plt : { *(.rel.plt) }
	.rela.plt : { *(.rela.plt) }
	.plt : { *(.plt) }

	/* .ARM.exidx is sorted, so has to go in its own output section.  */
	. = ALIGN(8);
	.ARM.exidx : { 
		__exidx_start = .;
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	}

	.rodata : ALIGN(4) {
		__rodata_start = .;
		*(.gnu.linkonce.r.*)
		*(.rodata .rodata.*)

		. = ALIGN(4);
		__rodata_end = .;
	}

	.data : ALIGN(4) {

		. = ALIGN(0x1000);
		__tsee_data_ro_start = .;
		KEEP(*(.data.tsee_ro))
		. = ALIGN(0x1000);
		__tsee_data_ro_end = .;

		__data_start = .;
		*(.data .data.* .gnu.linkonce.d.*)
		. = ALIGN(4);
	}

	.ctors : ALIGN(4) {
		__ctor_list = .;
		*(.ctors)
		*(.init_array)
		__ctor_end = .;
	}
	.dtors : ALIGN(4) {
		__dtor_list = .;
		*(.dtors)
		__dtor_end = .;
	}
	.got : { *(.got.plt) *(.got) }
	.dynamic : { *(.dynamic) }

	__data_end = .;
	/* unintialized data */
	.bss : ALIGN(4) {
		__bss_start = .;
		*(.bss .bss.*)
		*(.gnu.linkonce.b.*)
		*(COMMON)
		. = ALIGN(4);
		/*define a stack*/
		. = ALIGN(0x1000);
		. += 0x1000;
		PROVIDE (mon_stktop = .); 
		PROVIDE (tee_rw_stack_start = .); 
		. += 0x1000;
		PROVIDE (mon_stktop_init = .); 
		PROVIDE (tee_rw_stack_end = .); 
		. += 0x4000;
		PROVIDE (mon_stktop_others = .); 
		. += 0x1000;
		__bss_end = .;
	}

	. = ALIGN(0x1000);
	.tsee_rtks.phymap : { 
		tsee_rtks_map_bitmap = .;
		*(.tsee_rtks.phymap) 
		tsee_rtks_map_bitmap_end = .;
	}

	/* define the kernel page table, must be 16K and 16K-aligned*/
	. = ALIGN(0x4000);
	_tsee_pgtbl = .;
	. += 0x4000;

	_end = .;

	__init_mem_usage = _end - CFG_TSEE_LOAD_ADDR;
	. = CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE;

	/* 1MB aligment with TEE image */
	. = ALIGN(0x100000);
	PROVIDE (_binary_tee_img_start = .);
	tee_img : {
		*(tee_img)
	}
	PROVIDE (_binary_tee_img_end = .);
	_binary_tee_img_size = _binary_tee_img_end - _binary_tee_img_start;

	_end_of_ram = .;

	/* Strip unnecessary stuff */
	/DISCARD/ : { *(.comment .note .eh_frame) }

}
