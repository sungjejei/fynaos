#!/bin/bash

#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This file installs the os in the disk image file.
#
# Copyright (c) 2026 Seongjae Kim
#

if [ ! -s ".script-mount-info" ]; then
    echo "error: The disk image is not mounted."
    exit 1
fi

cp bin/fxos /mnt/boot/fxos
