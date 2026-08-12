# SPDX-License-Identifier: GPL-3.0-or-later

#
# This is the top-level Makefile for the FYNAOS project.
#
# Copyright (c) Seongjae Kim
#

#
# Variables
#

BIN := bin/
ISO := os.iso
IMG := fxos

CC := clang
LD := ld.lld
AS := clang
QEMU := qemu-system-x86_64

LDSCRIPT := scripts/linker.ld

CWARNS := -Wall                 \
          -Wextra               \
          -Werror               \
          -Wpedantic            \
          -Wconversion          \
          -Wsign-conversion     \
          -Wuninitialized       \
          -Wmissing-prototypes  \
          -Wstrict-prototypes   \
          -Wpointer-arith       \
          -Wcast-qual           \
          -Wunused              \
          -Wunused-function     \
          -Wunused-variable

CFLAGS := --target=x86_64-elf     \
          -ffreestanding          \
          $(CWARNS)               \
          -fno-builtin            \
          -nostdlib               \
          -fno-pic                \
          -fno-stack-protector    \
          -mno-red-zone           \
          -fno-pie                \
          -fno-omit-frame-pointer \
          -fcf-protection=none    \
          -mgeneral-regs-only     \
          -mno-sse                \
          -mno-sse2               \
          -mno-mmx                \
          -mno-avx                \
          -mcmodel=kernel         \
          -c                      \
          -Iinclude               \
          -g -Og                  \
          -MMD -MP

ASFLAGS := $(CFLAGS)

LDFLAGS := -T $(LDSCRIPT) -nostdlib

QEMUFLAGS := -bios /usr/share/ovmf/OVMF.fd       \
             -drive file=bin/disk.img,format=raw \
             -serial null                        \
             -serial null                        \
             -serial stdio                       \
             -d int -D qemu.log                  \
             -no-reboot

#
# Sources and Objects
#

CSOURCES := init/main.c rtl/snprintf.c rtl/string.c \
            kernel/kprintf.c kernel/panic.c kernel/interrupt.c \
            drivers/uart.c drivers/framebuffer.c drivers/fbcon.c \
            cpu/idt.c cpu/gdt.c \
            mm/boot.c mm/mm.c mm/pmm.c mm/pool.c mm/vmm.c

ASMSOURCES := boot/boot.S kernel/asm.S kernel/context.S

OBJECTS := $(addprefix $(BIN), $(CSOURCES:.c=.o)) $(addprefix $(BIN), $(ASMSOURCES:.S=.o))

#
# Verbose
#

V ?= 0

ifeq ($(V), 0)
Q := @
else
Q := 
endif

#
# PHONY
#

.PHONY: bindirs all clean img run run-debug

#
# Build Rules
#

all: bin/$(IMG)

-include $(OBJECTS:.o=.d)

bin/$(IMG): $(OBJECTS)
	@printf "LD  %s\n" "$@"
	$(Q)$(LD) $(LDFLAGS) $^ -o $@

bin/%.o: %.c | bindirs
	@printf "CC  %s\n" "$@"
	$(Q)$(CC) $(CFLAGS) $< -o $@

bin/%.o: %.S | bindirs
	@printf "AS  %s\n" "$@"
	$(Q)$(AS) $(ASFLAGS) $< -o $@

bin/%.o: %.s | bindirs
	@printf "AS  %s\n" "$@"
	$(Q)$(AS) $(ASFLAGS) $< -o $@

#
# Bin Directories
#

bindirs:
	$(Q)mkdir -p $(sort $(dir $(OBJECTS)))

#
# Clean
#

clean:
	rm -rf bin/*

#
# Disk Image
#

img: bin/disk.img

bin/disk.img: bin/$(IMG) 
	@printf "IMG %s\n" bin/disk.img
	$(Q)sudo scripts/update-image.sh > /dev/null 2>&1

#
# VM Execution
#

run: img
	@echo RUN
	$(Q)$(QEMU) $(QEMUFLAGS)

run-debug: img
	@echo RUN
	$(Q)$(QEMU) $(QEMUFLAGS) -S -s
