#!/bin/bash


# uefi-run -b OVMF.fd -q /usr/bin/qemu-system-x86_64 VirtualBiosMod.efi
export StartupDelay=0
./tools/uefi-run -b ./tools/OVMF.fd VirtualBiosMod.efi
