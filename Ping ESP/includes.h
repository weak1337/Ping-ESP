#include <Windows.h>
#include <TlHelp32.h>
#include <intrin.h>
#include <iostream>
#include <string>
#include <vector>
#include "mem.h"

class R6 {
public:
	uintptr_t base;
	DWORD pid;
	Memory* mem;
	R6();
};