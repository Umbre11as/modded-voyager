#pragma once

#define START_BOOT_APPLICATION_SIG "48 8B C4 48 89 58 20 44 89 40 18 48 89 50 10 48 89 48 08 55 56 57 41 54"

#define WINDOWS_BOOTMGFW_PATH L"\\efi\\microsoft\\boot\\bootmgfw.efi"
#define WINDOWS_BOOTMGFW_BACKUP_PATH L"\\efi\\microsoft\\boot\\bootmgfw.efi.backup"

typedef EFI_STATUS(EFIAPI* IMG_ARCH_START_BOOT_APPLICATION)(VOID*, VOID*, UINT32, UINT8, VOID*);

EFI_STATUS EFIAPI RestoreBootMgfw(UINT32* Index);
EFI_STATUS EFIAPI GetBootMgfwPath(/*UINT32 Index,*/ EFI_DEVICE_PATH_PROTOCOL** BootMgfwDevicePath);
EFI_STATUS EFIAPI InstallBootMgfwHooks(EFI_HANDLE ImageHandle);
