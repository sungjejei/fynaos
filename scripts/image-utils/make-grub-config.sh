#!/bin/bash

#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This script makes a grub configuration file
# in the image.
#
# Copyright (c) 2026 Seongjae Kim
#

if [ ! -s ".script-mount-info" ]; then
    echo "error: The disk image is not mounted."
    exit 1
fi

mkdir -p /mnt/boot/grub

cat > /mnt/boot/grub/grub.cfg <<EOF
set default=0
set timeout=60

menuentry "FYNAOS" {
    multiboot2 /boot/fxos
    boot
}
EOF
