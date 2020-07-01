#include <efi.h>
#include <efilib.h>


static EFI_STATUS get_bios_variables(const EFI_GUID *vendor, CHAR16 *name, CHAR8 **buffer, UINTN *size, UINT32 *attributes) {
        CHAR8 *buf;
        UINTN l;
        EFI_STATUS err;

        l = sizeof(CHAR16 *) * EFI_MAXIMUM_VARIABLE_SIZE;
        buf = AllocatePool(l);
        if (!buf)
                return EFI_OUT_OF_RESOURCES;

        err = uefi_call_wrapper(RT->GetVariable, 5, name, vendor, attributes, &l, buf);
        if (EFI_ERROR(err) == EFI_SUCCESS) {
                *buffer = buf;
                if (size)
                        *size = l;
        } else
                FreePool(buf);
        return err;
}


static EFI_STATUS set_bios_variables(CHAR16 *name, const EFI_GUID *vendor, UINTN size, CHAR8 *buffer) {
    EFI_STATUS status;
    status = uefi_call_wrapper(RT->SetVariable, 5, name, vendor, 3, size, buffer);
    return status;
}


EFI_STATUS efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
    int changes = 0;
    EFI_STATUS status;
    EFI_GUID guid = { 0xEC87D643, 0xEBA4, 0x4BB5, { 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 } };

    CHAR8 *data;
    UINTN data_size;
    UINT32 *attr = 0;

    InitializeLib(image, systab);

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);


    EFI_INPUT_KEY efi_input_key;
    EFI_STATUS efi_status;
    BOOLEAN exit = FALSE;

/////////--DRAW MAIN BLUE BOX--/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Print(L"                 SERDELIUK - VirtualBiosMod CMOS Setup Utility                  ");
    Print(L"%c",BOXDRAW_DOWN_RIGHT_DOUBLE);
	for(int i = 0; i < 78; i++) {
	    Print(L"%c",BOXDRAW_DOUBLE_HORIZONTAL);
	};
    Print(L"%c",BOXDRAW_DOWN_LEFT_DOUBLE);

    for(int w=0; w<11 ; w++){
	Print(L"%c",BOXDRAW_VERTICAL);
	    for(int i = 0; i < 78; i++) {
		Print(L" ");
	    };
	Print(L"%c",BOXDRAW_VERTICAL);
    };

    Print(L"%c",BOXDRAW_UP_RIGHT_DOUBLE);
	for(int i = 0; i < 78; i++) {
	    Print(L"%c",BOXDRAW_DOUBLE_HORIZONTAL);
	};
    Print(L"%c",BOXDRAW_UP_LEFT_DOUBLE);
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
        return EFI_SUCCESS;
    }

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 6); // h, v ;pos
    Print(L"Size:                   %d " , data_size);

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
    Print(L"Attributes no:              %d " , attr);

    int offset_lock = 0xb4b;
    int offset_video = 0xa45;

redraw:
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);
    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 8); // h, v ;pos
    Print(L"Bios Lock at 0x%02x is:  0x%02x", offset_lock, data[offset_lock]);
    if ( data[offset_lock] == 0) {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 9); // h, v ;pos
	Print(L"B.I.O.S. status:        Unlocked");
	} else {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 9); // h, v ;pos
	Print(L"B.I.O.S. status:        Locked");
    }

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 10); // h, v ;pos
    Print(L"Video at 0x%02x is:      0x%02x", offset_video, data[offset_video]);
    if ( data[offset_video] == 0) {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 11); // h, v ;pos
	Print(L"Video card:             IGFX -- Intel ");
	} else {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 11); // h, v ;pos
	Print(L"Video card:             SG   -- Nvidia");
    }

    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLACK);
    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 15);
    Print(L"\n\n Press V to switch video card\n");
    Print(L" Press B to unlock the bios\n");



    Print(L" Press ENTER to save new settings\n");
    if ( changes == 0 ) {
	Print(L" Press any other key or wait to boot without any mods\n");
    } else {
	Print(L" Press X to boot without any mods                    \n");
    }

    WaitForSingleEvent(ST->ConIn->WaitForKey, 40000000); // 10000000 = one second

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
	    goto redraw;
        case 'b':
	changes=1;
	    if ( data[offset_lock] == 0) {
		data[offset_lock] = 0x1;
	    } else {
		data[offset_lock] = 0x0;
	    }
	    goto redraw;
        case 'x':
    	    Print(L" Exiting......\n");
    	    WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
    	    return EFI_SUCCESS;
        case CHAR_CARRIAGE_RETURN:
	    if ( changes == 0 ) {
    		Print(L" Nothing to save, booting......\n");
    		WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
    		return EFI_SUCCESS;
	    }
	    status = set_bios_variables(L"Setup", &guid, data_size, data);
	    if (status != EFI_SUCCESS) {
		uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
		Print(L" ERROR saving data %r\n" , status);
		WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
    		return EFI_SUCCESS;
	    }
		uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_GREEN|EFI_BACKGROUND_BLACK);
	    Print(L" Write data OK, rebooting...          \n");
    	    WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    uefi_call_wrapper(RT->ResetSystem, 4, EfiResetCold, EFI_SUCCESS, 0, NULL);
    	    return EFI_SUCCESS;
        default: // continue boot
	    if ( changes == 1 ) {
		goto redraw;
	    }
    	    Print(L" Exiting......\n");
    	    WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
    	    return EFI_SUCCESS;
        }
    }
    	    return EFI_SUCCESS;
}
