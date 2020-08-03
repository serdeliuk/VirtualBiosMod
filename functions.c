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

        Please keep this header in all your source code.
*/


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

static EFI_STATUS draw_box_simple(int width, int lines, int h, int v) {
    EFI_STATUS status;
    int length = width - 2;

    status = uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, h, v); // h, v ;pos

    Print(L"%c",BOXDRAW_DOWN_RIGHT_DOUBLE);
	for(int i = 0; i < length; i++) {
	    Print(L"%c",BOXDRAW_DOUBLE_HORIZONTAL);
	};
	Print(L"%c",BOXDRAW_DOWN_LEFT_DOUBLE);

    for(int w=0; w<lines ; w++){
	v++;
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, h, v); // h, v ;pos
        Print(L"%c",BOXDRAW_VERTICAL);
            for(int i = 0; i < length; i++) {
                Print(L" ");
            };
        Print(L"%c",BOXDRAW_VERTICAL);
    };

	v++;
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, h, v); // h, v ;pos
    Print(L"%c",BOXDRAW_UP_RIGHT_DOUBLE);
        for(int i = 0; i < length; i++) {
            Print(L"%c",BOXDRAW_DOUBLE_HORIZONTAL);
        };
    Print(L"%c",BOXDRAW_UP_LEFT_DOUBLE);
    return status;
}



static EFI_STATUS draw_box(CHAR16 *title, int lines, int h, int v) {
    EFI_STATUS status;
    status = uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, h, v); // h, v ;pos
    int length = StrLen(title) + 6;

////////// title bar
    Print(L"%c",BOXDRAW_DOWN_RIGHT_DOUBLE);
        for(int i = 0; i < 2; i++) {
            Print(L"%c",BOXDRAW_DOUBLE_HORIZONTAL);
        };
	Print(L" %s ", title);
	    for(int i = 0; i < 2; i++) {
        	Print(L"%c",BOXDRAW_DOUBLE_HORIZONTAL);
	    };
    Print(L"%c",BOXDRAW_DOWN_LEFT_DOUBLE);
//////////////
    for(int w=0; w<lines ; w++){
	v++;
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, h, v); // h, v ;pos
        Print(L"%c",BOXDRAW_VERTICAL);
            for(int i = 0; i < length; i++) {
                Print(L" ");
            };
        Print(L"%c",BOXDRAW_VERTICAL);
    };

	v++;
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, h, v); // h, v ;pos
    Print(L"%c",BOXDRAW_UP_RIGHT_DOUBLE);
        for(int i = 0; i < length; i++) {
            Print(L"%c",BOXDRAW_DOUBLE_HORIZONTAL);
        };
    Print(L"%c",BOXDRAW_UP_LEFT_DOUBLE);
    return status;
}


static EFI_STATUS GetCursorPosition( IN UINTN *h, IN UINTN *v) {
    *h = gST->ConOut->Mode->CursorColumn;
    *v = gST->ConOut->Mode->CursorRow;

    return 0;
}

static EFI_STATUS draw_box_menu(CHAR16 *title, int lines, int h, int v ) {
    EFI_STATUS status;
    EFI_INPUT_KEY efi_input_key;
    EFI_INPUT_KEY KeyReset = {0};

    BOOLEAN exit = FALSE;

    status = uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, h, v); // h, v ;pos
    int length = StrLen(title) + 6;
    UINTN pos = v;
////////// title bar
    Print(L"%c",BOXDRAW_DOWN_RIGHT_DOUBLE);
        for(int i = 0; i < 2; i++) {
            Print(L"%c",BOXDRAW_DOUBLE_HORIZONTAL);
        };
	Print(L" %s ", title);
	    for(int i = 0; i < 2; i++) {
        	Print(L"%c",BOXDRAW_DOUBLE_HORIZONTAL);
	    };
    Print(L"%c",BOXDRAW_DOWN_LEFT_DOUBLE);
//////////////
    for(int w=0; w<lines ; w++){
	v++;
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, h, v); // h, v ;pos
        Print(L"%c",BOXDRAW_VERTICAL);
            for(int i = 0; i < length; i++) {
                Print(L" ");
            };
        Print(L"%c",BOXDRAW_VERTICAL);
    };

	v++;
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, h, v); // h, v ;pos
    Print(L"%c",BOXDRAW_UP_RIGHT_DOUBLE);
        for(int i = 0; i < length; i++) {
            Print(L"%c",BOXDRAW_DOUBLE_HORIZONTAL);
        };
    Print(L"%c",BOXDRAW_UP_LEFT_DOUBLE);

    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_RED);

    int menu_index = 0;
    CHAR8 menu_item_0 = "item 0";
    CHAR8 menu_item_1 = "item 1";
    CHAR8 menu_item_2 = "item 2";

redraw_menu:
    WaitForSingleEvent(ST->ConIn->WaitForKey, 10); // 10000000 = one second

    exit = FALSE;
    while (!exit) {

    status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &efi_input_key);


    switch (efi_input_key.UnicodeChar) {
        case CHAR_CARRIAGE_RETURN:
	    PrintAt(0,0,L"ENTER   ");
            break;
        case 'x':
	    PrintAt(0,0,L"x       ");
	    return 0;
        default:
            break;
    }

    switch (efi_input_key.ScanCode) {
        case SCAN_ESC:
	    PrintAt(0,0,L"ESC     ");
            break;
        case SCAN_UP:
	    PrintAt(0,0,L"UP      ");
            break;
        case SCAN_DOWN:
	    PrintAt(0,0,L"DOWN    ");
            break;
        default:
            break;
    }
            exit = TRUE;
    }

    goto redraw_menu;
    return status;
}
