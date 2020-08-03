#        This file is part of VirtualBiosMod, a virtual bios setup interface
#        Copyright (C) 2020 Alexandru Marc Serdeliuc
#
#        https://github.com/serdeliuk/VirtualBiosMod
#
#        VirtualBiosMod is free software: you can redistribute it and/or modify
#        it under the terms of the GNU General Public License as published by
#        the Free Software Foundation, either version 3 of the License, or
#        (at your option) any later version.
#
#        VirtualBiosMod is distributed in the hope that it will be useful,
#        but WITHOUT ANY WARRANTY; without even the implied warranty of
#        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#        GNU General Public License for more details.
#
#        You should have received a copy of the GNU General Public License
#        along with VirtualBiosMod.  If not, see <http://www.gnu.org/licenses/>.


ARCH            = $(shell uname -m)

OBJS            = VirtualBiosMod.o
TARGET          = VirtualBiosMod.efi

EFI_INC          = /usr/include/efi
EFI_LIB          = /usr/lib
EFI_CRT_OBJS    = $(EFI_LIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFI_LIB)/elf_$(ARCH)_efi.lds

CFLAGS          = -nostdlib \
                  -fno-stack-protector \
                  -fno-strict-aliasing \
                  -fno-builtin \
                  -fpic \
                  -fshort-wchar \
                  -mno-red-zone \
                  -Wall

ifeq ($(ARCH),x86_64)
  CFLAGS        += -DEFI_FUNCTION_WRAPPER
endif

CFLAGS          += -I$(EFI_INC) \
                   -I$(EFI_INC)/$(ARCH) \
                   -I$(EFI_INC)/protocol

LDFLAGS         = -nostdlib \
                  -znocombreloc \
                  -shared \
                  -no-undefined \
                  -Bsymbolic

LDFLAGS         += -T $(EFI_LDS) \
                   -L$(EFI_LIB) \
                   $(EFI_CRT_OBJS)

LIBS            = -lefi \
                  -lgnuefi

OBJCOPYFLAGS    = -j .text \
                  -j .sdata \
                  -j .data \
                  -j .dynamic \
                  -j .dynsym \
                  -j .rel \
                  -j .rela \
                  -j .reloc \
                  --target=efi-app-$(ARCH)

all: $(TARGET)

VirtualBiosMod.so: $(OBJS)
		  ld $(LDFLAGS) $(OBJS) -o $@ $(LIBS)

%.efi: %.so
		  objcopy $(OBJCOPYFLAGS) $^ $@

.PHONY:    clean

clean:
		  rm -f $(OBJS) $(TARGET) VirtualBiosMod.so
