#!/bin/bash

#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This script makes image, makes partitions
# and makes filesystems.
# This scripts must be run as supervisor.
#
# Copyright (c) 2026 Seongjae Kim
#

dd if=/dev/zero of=bin/disk.img bs=1G count=1
chown $SUDO_USER ./bin/disk.img

parted bin/disk.img mklabel gpt
parted bin/disk.img mkpart "efi-system-partition" fat32 1MiB 100MiB
parted bin/disk.img mkpart "main-partition" ext2 100MiB 100%

LOOP_DEVICE=$(losetup --find --partscan --show bin/disk.img)

mkfs.vfat -F 32 ${LOOP_DEVICE}p1
mkfs.ext2 ${LOOP_DEVICE}p2

losetup -d $LOOP_DEVICE
