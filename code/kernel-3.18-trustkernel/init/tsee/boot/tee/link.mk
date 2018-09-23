link-out-dir = $(out-dir)

link-script = $(platform-dir)/boot.ld.S
link-script-pp = $(link-out-dir)/boot.ld
link-script-dep = $(link-out-dir)/.boot.ld.d

AWK	 = awk


link-ldflags  = $(LDFLAGS)
link-ldflags += -T $(link-script-pp) -Map=$(link-out-dir)/tsee.map
link-ldflags += --sort-section=alignment
link-ldflags += --fatal-warnings
link-ldflags += --print-gc-sections

link-ldadd  = $(LDADD)
link-ldadd += $(addprefix -L,$(libdirs))
link-ldadd += $(addprefix -l,$(libnames))
ldargs-tsee.elf := $(link-ldflags) $(objs) $(link-out-dir)/version.o \
	$(link-ldadd) $(libgcc)

link-script-cppflags := -DASM=1 \
	$(filter-out $(CPPFLAGS_REMOVE) $(cppflags-remove), \
		$(nostdinc) $(CPPFLAGS) \
		$(addprefix -I,$(incdirs) $(link-out-dir)) \
		$(cppflags))

-include $(link-script-dep)

link-script-extra-deps += $(conf-file)
cleanfiles += $(link-script-pp) $(link-script-dep)
$(link-script-pp): $(link-script) $(link-script-extra-deps)
	@$(cmd-echo-silent) '  CPP     $@'
	@mkdir -p $(dir $@)
	$(q)$(CPP) -Wp,-P,-MT,$@,-MD,$(link-script-dep) \
		$(link-script-cppflags) $< > $@

define update-buildcount
	@$(cmd-echo-silent) '  UPD     $(1)'
	$(q)if [ ! -f $(1) ]; then \
		mkdir -p $(dir $(1)); \
		echo 1 >$(1); \
	else \
		expr 0`cat $(1)` + 1 >$(1); \
	fi
endef

version-o-cflags = $(filter-out -g3,$(platform-cflags) \
			$(platform-cflags)) # Workaround objdump warning
DATE_STR = `date -u`
BUILD_COUNT_STR = `cat $(link-out-dir)/.buildcount`
define gen-version-o
	$(call update-buildcount,$(link-out-dir)/.buildcount)
	@$(cmd-echo-silent) '  GEN     $(link-out-dir)/version.o'
	$(q)echo -e "const char v_str[] =" \
		"\"$(CFG_TSEE_IMPL_VERSION) \"" \
		"\"#$(BUILD_COUNT_STR) \"" \
		"\"$(DATE_STR) \"" \
		"\"$(ARCH)\";\n" \
		| $(CC) $(version-o-cflags) \
			-xc - -c -o $(link-out-dir)/version.o
endef
$(link-out-dir)/version.o:
	$(call gen-version-o)

all: $(link-out-dir)/tsee.elf
cleanfiles += $(link-out-dir)/tsee.elf $(link-out-dir)/tsee.map
cleanfiles += $(link-out-dir)/version.o
cleanfiles += $(link-out-dir)/.buildcount
$(link-out-dir)/tsee.elf: $(objs) $(libdeps) $(link-script-pp)
	$(call gen-version-o)
	@$(cmd-echo-silent) '  LD      $@'
	$(q)$(LD) $(ldargs-tsee.elf) -o $@

all: $(link-out-dir)/tsee.dmp
cleanfiles += $(link-out-dir)/tsee.dmp
$(link-out-dir)/tsee.dmp: $(link-out-dir)/tsee.elf
	@$(cmd-echo-silent) '  OBJDUMP $@'
	$(q)$(OBJDUMP) -l -x -d $< > $@

$(link-out-dir)/tsee.bin: $(link-out-dir)/tsee.elf
	@$(cmd-echo-silent) '  OBJCOPY $@'
	$(q)$(OBJCOPY) -O binary \
		$< $@

cleanfiles += $(link-out-dir)/tsee-init_load_addr.txt
$(link-out-dir)/tsee-init_load_addr.txt: $(link-out-dir)/tsee.elf
	@$(cmd-echo-silent) '  GEN     $@'
	@echo -n 0x > $@
	$(q)$(NM) $< | grep ' _start' | sed 's/ .*$$//' >> $@

cleanfiles += $(link-out-dir)/tsee-init_mem_usage.txt
$(link-out-dir)/tsee-init_mem_usage.txt: $(link-out-dir)/tsee.elf
	@$(cmd-echo-silent) '  GEN     $@'
	@echo -n 0x > $@
	$(q)$(NM) $< | grep ' __init_mem_usage' | sed 's/ .*$$//' >> $@

all: $(link-out-dir)/tsee.bin
cleanfiles += $(link-out-dir)/tsee.bin

all: $(link-out-dir)/tsee.symb_sizes
cleanfiles += $(link-out-dir)/tsee.symb_sizes
$(link-out-dir)/tsee.symb_sizes: $(link-out-dir)/tsee.elf
	@$(cmd-echo-silent) '  GEN     $@'
	$(q)$(NM) --print-size --reverse-sort --size-sort $< > $@

