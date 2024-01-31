# VirtualBiosMod

#### Bios MOD without hardware flashing requirement
[![download](https://img.shields.io/github/downloads/serdeliuk/VirtualBiosMod/total)](https://github.com/serdeliuk/VirtualBiosMod/releases/download/8/VirtualBiosMod.1.0.8.zip)

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/serdeliuk) any donation is highly appreciated!

## This source code is licensed under [GNU GPLv3](https://www.gnu.org/licenses/gpl-3.0.html#preamble)
## Please read LICENSE.txt for more details


This virtual mod is not permament and if your new settings goes bad you need to remove cmos battery to reset your settings to factory default.

It was tested only on Asus ROG STRIX G531GT 

This should work with any UEFI bios regardless the operating system, if you have Clover booloader put the VirtualBioosMod.efi in drivers and reboot.<br>
If your bootloader does not know how to load efi drivers put the VirtualBiosMod.efi on a fat32 usb stick and create in your bios a new boot entry and point to VirtualBiosMod.efi file.<br>
Put the new entry as first boot and the second boot put OS which want to boot after exit from VirtualBiosMod.efi app
Please refer to [Wiki](https://github.com/serdeliuk/VirtualBiosMod/wiki) for more details

This version modify your bios CMOS settings, you can change bios lock and / or disable nvidia video card.

Use this thread for comments [Virtual Bios Mod without flashing](https://www.insanelymac.com/forum/topic/358607-tool-virtualbiosmod-safely-mod-your-firmware-bios-without-flashing-requirement/)
<br><br>

Virtual Bios MOD boot screen
---
![VirtualBiosMod](https://github.com/serdeliuk/VirtualBiosMod/blob/master/img/VirtualBiosMod.1.0.13.gif)

### Special thanks to
- @LongSoft for [Universal-IFR-Extractor](https://github.com/LongSoft/Universal-IFR-Extractor) and [UEFItool](https://github.com/LongSoft/UEFITool)
- @Zibri for [afulnx](https://github.com/Zibri/afulnx)
- @RomanHargrave for [amifldrv](https://github.com/RomanHargrave/amifldrv)
- @Richard-W for [uefi-run](https://github.com/Richard-W/uefi-run)
- @platomav for [BIOSUtilities](https://github.com/platomav/BIOSUtilities)

I hope that will help in your future projects<br><br>
Have fun!

