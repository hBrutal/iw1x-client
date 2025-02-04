#include "std_include.hpp"

#pragma comment(linker, "/merge:.text=._text")
#pragma comment(linker, "/merge:.rdata=._rdata")
#pragma comment(linker, "/merge:.data=._data")

#pragma bss_seg(".payload")
char payload_data[BINARY_PAYLOAD_SIZE];

extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;
};