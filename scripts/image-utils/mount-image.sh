#!/bin/bash

#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This script creates loop devices for the image file
# and mount them.
# Before execution of this script, the image file must be
# initialized.
#
# Copyright (c) 2026 Seongjae Kim
#

set -e

if [ -s ".script-mount-info" ]; then
    echo "error: Already mounted."
    exit 1
fi

LOOP_DEVICE=$(losetup --find --partscan --show bin/disk.img)

printf "$LOOP_DEVICE" > .script-mount-info

mount ${LOOP_DEVICE}p2 /mnt
mkdir -p /mnt/boot/efi
mount ${LOOP_DEVICE}p1 /mnt/boot/efi
