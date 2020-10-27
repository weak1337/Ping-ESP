#include "includes.h"
R6::R6() {
	this->mem = new Memory();
}
R6* r6;
int main() {
	r6 = new R6();
	r6->mem->setup();

	/*
	Drone-Check patch
	old xor eax,eax
	new nop, nop
	*/
	uintptr_t to_patch1 = r6->base + 0x245b825; //31 c0 41 88 84 24 ? ? ? ? 48 8b 0d ? ? ? ? 48 8b 81
	static byte nops[] = { 0x90, 0x90 };
	r6->mem->cwrite(to_patch1, nops, sizeof(nops), true);

	/*
	Key-Check patch
	old cmp edx,000000DD
	new jmp, destinaton
	-> We jump over the key check
	*/
	uintptr_t to_patch2 = r6->base + 0x1e78e78; //81 fa ? ? ? ? 7e ? 81 fa ? ? ? ? 7e ? 81 fa ? ? ? ? 0f 84
	uintptr_t address_to_jump_to = r6->base + 0x1e78f5f; //48 8b 05 ? ? ? ? 48 8b 80 ? ? ? ? 48 85 c0 0f 84 ? ? ? ? 48 8b 88 ? ? ? ? 48 85 c9
	DWORD relative = address_to_jump_to - to_patch2 - 5;
	static uint8_t jmp[] = { 0xE9, 00, 00, 00, 00 };
	*(DWORD*)(&jmp[1]) = relative;
	r6->mem->cwrite(to_patch2, jmp, sizeof(jmp), true);

	/*
	Set scan time/delay to 0
	*/
	uintptr_t time_base = r6->mem->read<uintptr_t>(r6->base + 0x7040400);
	uintptr_t time_ref = r6->mem->read<uintptr_t>(time_base + 0x18);
	time_ref -= 3;
	time_ref = _rotl64(time_ref, 0xC);
	time_ref += 0x7BBD8DB4D2A00CAA;
	time_ref = r6->mem->read<uintptr_t>(time_ref);
	r6->mem->write<float>(time_ref + 0x5430, 0.f);
	
	/*
	IsVisible patch
	old je RainbowSix.exe+375FACB
	new jne RainbowSix.exe+375FACB
	*/
	uintptr_t to_patch3 = r6->base + 0x375fa57; //74 ? 48 8b 05 ? ? ? ? 48 8d 0d ? ? ? ? ba ? ? ? ? ff 50 ? 48 85 c0 74 ? f0 83 05 ? ? ? ? ? c7 40 ? ? ? ? ? 0f 57 c0 0f 11 40 ? c6 40 ? ? 48 8d 0d ? ? ? ? 66 48 0f 6e c1 48 8d 0d ? ? ? ? 66 48 0f 6e c9 66 0f 6c c8 f3 0f 7f 08 48 c7 40
	BYTE patch2 = 0x75;
	r6->mem->cwrite(to_patch3, &patch2, sizeof(patch2), true);

	system("pause");
}