# FYNAOS

FYNAOS is an operating system written from scratch.

## Main Characteristics

- C-based Operating System
- Makefile-based Buildsystem
- Using GRUB Bootloader

## Requirements

- GCC or Clang
- LLVM or GNU Binutils
- GRUB Tools
- GNU Make

## Build

### Build the kernel with Make:

```
make
```

### Make a disk image file:

> Warning: This rule uses `sudo`.
> You may be prompted to enter your password.

```
make img
```

### Run the kernel with QEMU:

```
make run
```

### Debug the kernel with QEMU and GDB/LLDB:

```
make run-debug
```

## Features

- [x] Build system
- [x] Boot the kernel with GRUB
- [x] Basic Interrupt Handling
- [ ] Handling hardware interrupts using PIC
- [ ] Memory management
- [ ] Scheduling
- [ ] User mode
