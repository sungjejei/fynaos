#!/bin/bash

#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This script is the orchestrator for image files.
#
# Copyright (c) 2026 Seongjae Kim
#

if [ "$EUID" -ne 0 ]; then
    echo "scripts/update-image: must be run as root" >&2
    exit 1
fi

if [ ! -e bin/disk.img ]; then
    scripts/image-utils/init-image.sh
fi

scripts/image-utils/mount-image.sh

scripts/image-utils/install-grub.sh
scripts/image-utils/install-os.sh
scripts/image-utils/make-grub-config.sh

scripts/image-utils/umount-image.sh
