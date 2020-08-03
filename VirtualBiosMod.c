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
        along with VirtualBiosMod.  If not, see <http://www.gnu.org/licenses/>

        Please keep this header in all your source codes.
*/


#include <efi.h>
#include <efilib.h>
#include "functions.c"


EFI_STATUS efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
    int changes = 0;
    int vmajor = 1;
    int vminor = 0;
    int vpatch = 6;
    int params = 0;

    EFI_STATUS status;
    EFI_GUID guid = { 0xEC87D643, 0xEBA4, 0x4BB5, { 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 } };

    CHAR8 *data;
    UINTN data_size;
    UINT32 *attr = 0;
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
//            Print(L"[VirtualBiosMod] params: %d\n", params);
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


/////////--DRAW MAIN BLUE BOX--/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Print(L"             SERDELIUK - VirtualBiosMod v%d.%d.%d CMOS Setup Utility               ",vmajor,vminor,vpatch);
    draw_box_simple(80, 11, 0, 1);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 3); // h, v ;pos
    Print(L"UEFI version:           %d.%02d", ST->Hdr.Revision >> 16, ST->Hdr.Revision & 0xffff);

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 4); // h, v ;pos
    Print(L"Firmware vendor:        %s", ST->FirmwareVendor,L"%c");

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
    Print(L"Firmware version:       %d.%02d", ST->FirmwareRevision >> 16, ST->FirmwareRevision & 0xffff);

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 15);
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

    int offset_lock = 0xb4b;
    int offset_video = 0xa45;
    int offset_audio = 0x108E;

redraw:
    WaitForSingleEvent(ST->ConIn->WaitForKey, 10); // 10000000 = one second
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);
    if ( data[offset_lock] == 0) {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
	Print(L"B.I.O.S. status:        Unlocked");
	} else {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
	Print(L"B.I.O.S. status:        Locked  ");
    }

    if ( data[offset_video] == 0) {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 8); // h, v ;pos
	Print(L"Video card:             IGFX -- Intel ");
	} else {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 8); // h, v ;pos
	Print(L"Video card:             SG   -- Nvidia");
    }

    if ( data[offset_audio] == 0) {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 9); // h, v ;pos
	Print(L"HDMI Audio:             Enabled       ");
	} else {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 9); // h, v ;pos
	Print(L"HDMI Audio:             Disabled      ");
    }

    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLACK);
    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 15);
    Print(L"\n\n Press V to switch video card\n");
    Print(L" Press B to unlock the bios\n");

    Print(L" Press D to draw a box\n");
    Print(L" Press H to enable/disable HDMI audio\n");


    Print(L" Press ENTER to save new settings\n");
    if ( changes == 0 ) {
	Print(L" Press any other key or wait to boot without any mods\n");
    } else {
	Print(L" Press X to boot without any mods                    \n");
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
        case 'h':
	changes=1;
	    if ( data[offset_audio] == 0) {
		data[offset_audio] = 0x1;
	    } else {
		data[offset_audio] = 0x0;
	    }
	    efi_input_key = KeyReset;
	    goto redraw;
        case 'd':
	    draw_box(L"Testing 123", 5, 60, 4);
	    goto redraw;
        case 'x':
    	    Print(L" Exiting......\n");
    	    WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    if ( params == 0){ 
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
        case CHAR_CARRIAGE_RETURN:
	    if ( changes == 0 ) {
    		Print(L" Nothing to save, booting......\n");
    		WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    if ( params == 0){ 
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
	    }
	    status = set_bios_variables(L"Setup", &guid, data_size, data);
	    if (status != EFI_SUCCESS) {
		uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
		Print(L" ERROR saving data %r\n" , status);
		WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    if ( params == 0){ 
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
	    }
		uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_GREEN|EFI_BACKGROUND_BLACK);
	    Print(L" Write data OK, rebooting...          \n");
    	    WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    uefi_call_wrapper(RT->ResetSystem, 4, EfiResetCold, EFI_SUCCESS, 0, NULL);
    	    return EFI_SUCCESS;
        default: // continue boot
	    if ( changes == 1 ) {
		efi_input_key = KeyReset;
		goto redraw;
	    }
    	    Print(L" Exiting......\n");
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
