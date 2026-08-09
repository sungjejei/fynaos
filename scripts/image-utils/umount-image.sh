#!/bin/bash

#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This script cleans all resources made by
# 'scripts/image-utils/mount-image.sh'.
#
# Copyright (c) Seongjae Kim
#

if [ ! -s ".script-mount-info" ]; then
    echo "error: Not mounted."
    exit 1
fi

LOOP_DEVICE=$(cat .script-mount-info)
: > .script-mount-info

umount /mnt/boot/efi
umount /mnt
losetup -d $LOOP_DEVICE
