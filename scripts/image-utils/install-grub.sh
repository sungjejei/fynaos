#!/bin/bash

#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This file installs the GRUB in the disk image file.
#
# Copyright (c) 2026 Seongjae Kim
#

if [ ! -s ".script-mount-info" ]; then
    echo "error: The disk image is not mounted."
    exit 1
fi

cat > bin/early-grub.cfg <<EOF
search.fs_uuid $(blkid -s UUID -o value $(cat .script-mount-info)p2) root
configfile /boot/grub/grub.cfg
EOF

grub-mkimage --format=x86_64-efi             \
             --output=bin/BOOTX64.EFI        \
             --config=bin/early-grub.cfg     \
             --prefix=/boot/grub             \
             boot chain configfile echo eval \
             ext2 fat font gettext gfxterm   \
             gzio help linux lsefi normal    \
             part_gpt read regexp search     \
             search_fs_file search_fs_uuid   \
             search_label terminal sleep     \
             test multiboot2 efi_gop

mkdir -p /mnt/boot/efi/EFI/BOOT/
cp bin/BOOTX64.EFI /mnt/boot/efi/EFI/BOOT/BOOTX64.EFI
