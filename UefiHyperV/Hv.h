#pragma once

#define INTEL_VMEXIT_HANDLER_SIG "65 C6 04 25 6D ?? ?? ?? ?? 48 8B 4C 24 ?? 48 8B 54 24 ?? E8 ?? ?? ?? ?? E9"

#define AMD_VMEXIT_HANDLER_SIG "E8 ?? ?? ?? ?? 48 89 04 24 E9"

#pragma pack(push, 1)
typedef struct _VOYAGER_T
{
	UINT64 VmExitHandlerRva;
	UINT64 HypervModuleBase;
	UINT64 HypervModuleSize;
	UINT64 ModuleBase;
	UINT64 ModuleSize;

	//UINT64 offset_vmcb_base;
	//UINT64 offset_vmcb_link;
	//UINT64 offset_vmcb;
} VOYAGER_T, * PVOYAGER_T;
#pragma pack(pop)



VOID* MapModule(PVOYAGER_T VoyagerData, UINT8* ImageBase);
VOID MakeVoyagerData(PVOYAGER_T VoyagerData, VOID* HypervAlloc, UINT64 HypervAllocSize, VOID* PayLoadBase, UINT64 PayLoadSize);
VOID* HookVmExit(VOID* HypervBase, VOID* HypervSize, VOID* VmExitHook);

