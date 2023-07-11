/*
        This file is part of VirtualBiosMod, a virtual bios setup interface
        Copyright (C) 2020 Alexandru Marc Serdeliuc

        https://github.com/serdeliuk/VirtualBiosMod

        VirtualBiosMod is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
        (at your option) any later version.

        VirtualBiosMod is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with VirtualBiosMod.  If not, see <http://www.gnu.org/licenses/>.

        Please keep this header in all your source code.
*/


#include <efi.h>
#include <efilib.h>
#include "functions.c"

EFI_STATUS efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
    int changes = 0;
    int vmajor = 1;
    int vminor = 0;
    int vpatch = 13;
    int params = 0;


// G531GT
    int offset_lock = 0xb4b; // BIOS Lock
    int offset_video = 0xa45; // Primary Display
    int offset_audio = 0x108E; // iDisplay Audio Disconnect
    int offset_xtu = 0x859; // XTU Interface
    int offset_overclock = 0x858; // OverClocking Feature
    int offset_ratio = 0x85A; //Core Max OC Ratio
    int offset_ring = 0x865; // Ring Max OC Ratio
    int offset_maxring = 0x8AC; // Max Ring Ratio Limit
    int offset_adaptive = 0x40F; // Adaptive Performance
    int offset_avx = 0x863; // AVX2 Ratio Offset
    int offset_oddratio = 0x92D; // QCLK Odd Ratio
    int offset_vmem = 0xA14;
    int offset_vmem_aperture = 0x962;

    int G531GT = 0x1472; // G531GT, G731GT // Size: 0x1472, Name: Setup 
//    int G512LU = 0x8F9;  // G512LU
//    int G531GW = 0x1476; // G531GW

    EFI_STATUS status;

    EFI_GUID guid = { 0xEC87D643, 0xEBA4, 0x4BB5, { 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 } };
    CHAR8 *data;
    UINTN data_size;
    UINT32 *attr = 0;

    EFI_GUID clockguid = { 0x64192DCA, 0xD034, 0x49D2, { 0xA6, 0xDE, 0x65, 0xA8, 0x29, 0xEB, 0x4C, 0x74 } };
    CHAR8 *clockdata;
    UINTN clockdata_size;
    UINT32 *clockattr = 0;

    EFI_INPUT_KEY efi_input_key;
    EFI_INPUT_KEY KeyReset = {0};

    EFI_STATUS efi_status;
    EFI_LOADED_IMAGE *loaded_image = NULL;
    BOOLEAN exit = FALSE;

    InitializeLib(image, systab);

    status = uefi_call_wrapper(systab->BootServices->HandleProtocol, 3, image, &LoadedImageProtocol, (void **) &loaded_image);
    if (EFI_ERROR(status)) {
            Print(L"[VirtualBiosMod] err: %r\n", status);
    } else {
		params = loaded_image->LoadOptionsSize;
    }

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    if ( params == 0 ){
    Print(L"\n\n\n[VirtualBiosMod v%d.%d.%d] Press ",vmajor,vminor,vpatch);
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);
    Print(L"del");
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLACK);
    Print(L" or any other key to enter Setup...");

    WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second

    while (!exit) {

    efi_status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &efi_input_key);

		if (efi_status != EFI_SUCCESS) {
		    Print(L" Exiting\n\n\n");
		    return EFI_SUCCESS;
		} else {
		    break;
		}
    }
    }

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);

//  Print(L"123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_");
/////////--DRAW MAIN BLUE BOX--/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Print(L"             SERDELIUK - VirtualBiosMod v%d.%d.%d CMOS Setup Utility               ",vmajor,vminor,vpatch);
    draw_box_simple(80, 8, 0, 1);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 2); // h, v ;pos
    Print(L"Firmware version:       %d.%02d", ST->FirmwareRevision >> 16, ST->FirmwareRevision & 0xffff);

    Print(L" (%s)", ST->FirmwareVendor,L"%c"); // firmware vendoor

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 3); // h, v ;pos
    Print(L"UEFI version:           %d.%02d", ST->Hdr.Revision >> 16, ST->Hdr.Revision & 0xffff);

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 12);

    status = get_bios_variables( &guid, L"Setup", &data, &data_size, attr);
    if (status != EFI_SUCCESS) {
		uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
        Print(L"Unsupported B.I.O.S.\n" , status);
		WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
		    if ( params == 0){ 
    				return EFI_SUCCESS;
		    } else {
				uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
		    }
    }


//		if ( Setupdata_size != G512LU) {
    if ( data_size != G531GT) {
        uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
        Print(L"Unsupported B.I.O.S. Setup size mismatch,  press any key\n");
        WaitForSingleEvent(ST->ConIn->WaitForKey, 100000000); // 10000000 = one second
            if ( params == 0){
                return EFI_SUCCESS;
            } else {
                uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
            }
    }

    status = get_bios_variables( &clockguid, L"IccAdvancedSetupDataVar", &clockdata, &clockdata_size, clockattr);
    if (status != EFI_SUCCESS) {
		uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
        Print(L"Unsupported B.I.O.S.\n" , status);
		WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
		    if ( params == 0){ 
    				return EFI_SUCCESS;
		    } else {
				uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
		    }
    }

redraw:
    WaitForSingleEvent(ST->ConIn->WaitForKey, 10); // 10000000 = one second
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);

    if ( data[offset_oddratio] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 4); // h, v ;pos
		Print(L"QCLK Odd Ratio:         Disabled      ");
		} else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 4); // h, v ;pos
		Print(L"QCLK Odd Ratio:         Enabled       ");
    }

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 4); // h, v ;pos
    if ( data[offset_vmem_aperture] == 0x0 ) {
		Print(L"VMEM size:                 128MB   ");
    }
    else if ( data[offset_vmem_aperture] == 0x1 ) {
		Print(L"VMEM size:                 256MB   ");
    }
    else if ( data[offset_vmem_aperture] == 0x3 ) {
		Print(L"VMEM size:                 512MB   ");
    }
    else if ( data[offset_vmem_aperture] == 0x7 ) {
		Print(L"VMEM size:                1024MB   ");
    }
    else if ( data[offset_vmem_aperture] == 0xF ) {
		Print(L"VMEM size:                2048MB   ");
    }
    else{
		Print(L"VMEM size:               UNKNOWN   ");
    }

    if ( data[offset_lock] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
		Print(L"B.I.O.S. status:        Unlocked      ");
		} else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
		Print(L"B.I.O.S. status:        Locked        ");
    }

    if ( data[offset_video] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 6); // h, v ;pos
		Print(L"Video card:             IGFX -- Intel ");
		} else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 6); // h, v ;pos
		Print(L"Video card:             SG   -- Nvidia");
    }

    if ( data[offset_audio] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
		Print(L"HDMI Audio:             Enabled       ");
		} else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
		Print(L"HDMI Audio:             Disabled      ");
    }

    if ( data[offset_xtu] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 8); // h, v ;pos
		Print(L"Intel  XTU:             Disabled   ");
		} else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 8); // h, v ;pos
		Print(L"Intel  XTU:             Enabled    ");
    }

    if ( data[offset_adaptive] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 9); // h, v ;pos
		Print(L"Adaptive performance:   Disabled   ");
		} else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 9); // h, v ;pos
		Print(L"Adaptive performance:   Enabled    ");
    }

    if ( data[offset_overclock] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 5); // h, v ;pos
		Print(L"Overclock :             Disabled   ");
		} else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 5); // h, v ;pos
		Print(L"Overclock :             Enabled    ");
    }

    if ( data[offset_ratio] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 6); // h, v ;pos
		Print(L"Core RATIO:             Default    ");
		} else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 6); // h, v ;pos
		Print(L"Core RATIO:             53         ");
    }

    if ( data[offset_ring] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 7); // h, v ;pos
		Print(L"Core Ring:              Default    ");
		} else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 7); // h, v ;pos
		Print(L"Core Ring:              53         ");
    }

    if ( data[offset_maxring] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 8); // h, v ;pos
		Print(L"Max Ring:               Default    ");
		} else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 8); // h, v ;pos
		Print(L"Max Ring:               53         ");
    }

    if ( data[offset_avx] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 9); // h, v ;pos
		Print(L"Max AVX:                Default    ");
		} else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 9); // h, v ;pos
		Print(L"Max AVX:                0x1F       ");
    }

    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLACK);
    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 11);
    Print(L" Press B to un/lock the bios\n");
    Print(L" Press F to change video memory size, press multiple times to see all options\n");
    Print(L" Press V to switch video card\n");
    Print(L" Press H to enable/disable HDMI audio\n");
    Print(L" Press I to enable/disable Intel XTU\n");
    Print(L" Press O to enable/disable Overclocking\n");
    Print(L" Press C to enable/disable CORE speed ratio\n");
    Print(L" Press R to enable/disable CORE ring ratio\n");
    Print(L" Press M to enable/disable MAX  ring ratio\n");
    Print(L" Press A to enable/disable adaptive ratio\n");
    Print(L" Press Q to enable/disable QCLK Odd Ratio\n");
    Print(L" Press X to enable/disable MAX  AVX ratio\n");

    if ( changes == 0 ) {
		Print(L" Press any other key or wait to boot without any mods\r");
    } else {
		Print(L" Press ENTER to save, or E to boot without any mods  \r");
    }

    WaitForSingleEvent(ST->ConIn->WaitForKey, 30000000); // 10000000 = one second

    while (!exit) {

    efi_status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &efi_input_key);

    switch (efi_input_key.UnicodeChar) {
        case 'v':
		changes=1;
		    if ( data[offset_video] == 0) {
				data[offset_video] = 0x4;
		    } else {
				data[offset_video] = 0x0;
		    }
		    efi_input_key = KeyReset;
		    goto redraw;
        case 'b':
		changes=1;
		    if ( data[offset_lock] == 0) {
				data[offset_lock] = 0x1;
		    } else {
				data[offset_lock] = 0x0;
		    }
		    efi_input_key = KeyReset;
		    goto redraw;
        case 'f':
		changes=1;
		    if ( data[offset_vmem_aperture] == 0x0) {
				data[offset_vmem_aperture] = 0x1;
		    }
		    else if ( data[offset_vmem_aperture] == 0x1) {
				data[offset_vmem_aperture] = 0x3;
		    }
		    else if ( data[offset_vmem_aperture] == 0x3) {
				data[offset_vmem_aperture] = 0x7;
		    }
		    else if ( data[offset_vmem_aperture] == 0x7) {
				data[offset_vmem_aperture] = 0xF;
		    }
		    else if ( data[offset_vmem_aperture] == 0xF) {
				data[offset_vmem_aperture] = 0x0;
		    }
		    else {
				data[offset_vmem] = 0x2;
				data[offset_vmem_aperture] = 0x1;
		    }
		    efi_input_key = KeyReset;
		    goto redraw;
        case 'h':
		changes=1;
		    if ( data[offset_audio] == 0) {
				data[offset_audio] = 0x1;
		    } else {
				data[offset_audio] = 0x0;
		    }
		    efi_input_key = KeyReset;
		    goto redraw;
        case 'i':
		changes=1;
		    if ( data[offset_xtu] == 0) {
				data[offset_xtu] = 0x1;
		    } else {
				data[offset_xtu] = 0x0;
		    }
		    efi_input_key = KeyReset;
		    goto redraw;
        case 'o':
		changes=1;
		    if ( data[offset_overclock] == 0) {
				data[offset_overclock] = 0x1;
		    } else {
				data[offset_overclock] = 0x0;
		    }
		    efi_input_key = KeyReset;
		    goto redraw;
        case 'c':
		changes=1;
		    if ( data[offset_ratio] == 0) {
				data[offset_ratio] = 0x53;
		    } else {
				data[offset_ratio] = 0x0;
		    }
		    efi_input_key = KeyReset;
		    goto redraw;
        case 'r':
		changes=1;
		    if ( data[offset_ring] == 0) {
				data[offset_ring] = 0x53;
		    } else {
				data[offset_ring] = 0x0;
		    }
		    efi_input_key = KeyReset;
		    goto redraw;
        case 'm':
		changes=1;
		    if ( data[offset_maxring] == 0) {
				data[offset_maxring] = 0x53;
		    } else {
				data[offset_maxring] = 0x0;
		    }
		    efi_input_key = KeyReset;
		    goto redraw;
        case 'a':
		changes=1;
		    if ( data[offset_adaptive] == 0) {
				data[offset_adaptive] = 0x1;
		    } else {
				data[offset_adaptive] = 0x0;
		    }
		    efi_input_key = KeyReset;
		    goto redraw;
        case 'x':
		changes=1;
		    if ( data[offset_avx] == 0) {
				data[offset_avx] = 0x1f;
		    } else {
				data[offset_avx] = 0x0;
		    }
		    efi_input_key = KeyReset;
		    goto redraw;
        case 'q':
		changes=1;
		    if ( data[offset_oddratio] == 0) {
				data[offset_oddratio] = 0x1;
		    } else {
				data[offset_oddratio] = 0x0;
		    }
		    efi_input_key = KeyReset;
		    goto redraw;
        case 'e':
    		    Print(L" Exiting......                                        \r");
    		    WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
		    if ( params == 0){ 
    				return EFI_SUCCESS;
		    } else {
				uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
		    }
        case CHAR_CARRIAGE_RETURN:
		    if ( changes == 0 ) {
    				Print(L" Nothing to save, booting......                      \r");
    				WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
		    if ( params == 0){ 
    				return EFI_SUCCESS;
		    } else {
				uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
		    }
		    }
		    status = set_bios_variables(L"Setup", &guid, *attr, data_size, data);
		    if (status != EFI_SUCCESS) {
				uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
				Print(L" ERROR saving Setup data %r                            \n" , status);
		    if ( params == 0){ 
    				return EFI_SUCCESS;
		    } else {
				uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
		    }
		    }
		    status = set_bios_variables(L"IccAdvancedSetupDataVar", &clockguid, *clockattr, clockdata_size, clockdata);
		    if (status != EFI_SUCCESS) {
				uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
				Print(L" ERROR saving Advanced Setup data %r                    \n" , status);
				WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
		    if ( params == 0){ 
    				return EFI_SUCCESS;
		    } else {
				uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
		    }
		    }
		    if ( params == 0){ 
                uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_GREEN|EFI_BACKGROUND_BLACK);
                Print(L" Write data OK, rebooting...                         \r");
                WaitForSingleEvent(ST->ConIn->WaitForKey, 20000000); // 10000000 = one second
    				return EFI_SUCCESS;
		    } else {
				uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_GREEN|EFI_BACKGROUND_BLACK);
				Print(L" Write data OK, rebooting...                         \r");
    				WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
				uefi_call_wrapper(RT->ResetSystem, 4, EfiResetCold, EFI_SUCCESS, 0, NULL);
    				return EFI_SUCCESS;
		    }
        default: // continue boot
		    if ( changes == 1 ) {
				efi_input_key = KeyReset;
				goto redraw;
		    }
    		    Print(L" Exiting......                                        \r");
    		    WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
		    if ( params == 0){ 
    				return EFI_SUCCESS;
		    } else {
				uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
		    }
        }
    }
    		    return EFI_SUCCESS;
}
