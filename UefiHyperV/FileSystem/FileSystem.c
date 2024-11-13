#include "FileSystem.h"

#include <Guid/FileInfo.h> // gEfiFileInfoGuid
#include <Library/UefiLib.h> // EFI_FILE_IO_INTERFACE
#include <Library/DevicePathLib.h> // FileDevicePath
#include <Library/UefiBootServicesTableLib.h> // gBS

static EFI_STATUS Alloc(IN UINTN Size, OUT VOID** Buffer) {
	return gBS->AllocatePool(EfiReservedMemoryType, Size, Buffer);
}

static EFI_STATUS Free(IN VOID* Buffer) {
	return gBS->FreePool(Buffer);
}

EFI_STATUS ReadFile(IN CHAR16* FileName, OUT UINTN* FileSize, OUT VOID* Buffer) {
	EFI_STATUS status = EFI_NOT_FOUND;

	UINTN count = 0;
	EFI_HANDLE* volumeHandles = NULL;
	if (EFI_ERROR((status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &count, &volumeHandles))))
		return status;

	BOOLEAN found = FALSE;
	// List volumes
	for (UINTN volumeIndex = 0; volumeIndex < count; volumeIndex++) {
		EFI_HANDLE volumeHandle = volumeHandles[volumeIndex];
		if (!volumeHandle)
			continue;

		// Open volume
		EFI_FILE_IO_INTERFACE* fileIO = NULL;
		if (EFI_ERROR(status = gBS->OpenProtocol(volumeHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID**)&fileIO, gImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL)))
			continue;

		EFI_FILE_HANDLE fileManager = NULL;
		if (EFI_ERROR(fileIO->OpenVolume(fileIO, &fileManager)))
			continue;

		// Open file
		EFI_FILE_HANDLE fileHandle;
		if (EFI_ERROR(status = fileManager->Open(fileManager, &fileHandle, FileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY)))
			continue;

		// Fetch file size
		UINTN fileInfoSize = 0;
		EFI_FILE_INFO* fileInfoBuffer = NULL;
		if (EFI_ERROR(status = fileHandle->GetInfo(fileHandle, &gEfiFileInfoGuid, &fileInfoSize, NULL))) {
			if (status == EFI_BUFFER_TOO_SMALL) // Get buffer size for file info
				Alloc(fileInfoSize, (VOID**) &fileInfoBuffer);
			else
				continue;
		}

		if (EFI_ERROR(status = fileHandle->GetInfo(fileHandle, &gEfiFileInfoGuid, &fileInfoSize, fileInfoBuffer)))
			continue;
		*FileSize = fileInfoBuffer->FileSize;

		// Read file
		if (!EFI_ERROR(status = fileHandle->Read(fileHandle, FileSize, Buffer)))
			found = TRUE;

		// Close file
		if (EFI_ERROR(status = fileManager->Close(fileHandle)))
			continue;

		// Close volume
		if (EFI_ERROR(status = gBS->CloseProtocol(volumeHandle, &gEfiSimpleFileSystemProtocolGuid, gImageHandle, NULL)))
			continue;

		if (found)
			break;
	}

	return status;
}
