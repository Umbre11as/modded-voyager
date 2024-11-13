#pragma once

typedef enum {
	Other,
	AMD,
	Intel
} CpuVendor;

CpuVendor GetCPUVendor();
bool CraftUefi(CpuVendor CupVendor);
