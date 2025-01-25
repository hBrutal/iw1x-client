#include "std_include.hpp"

#pragma comment(linker, "/base:0x400000")

#pragma comment(linker, "/merge:.text=.main")
#pragma comment(linker, "/merge:.rdata=.myrdata")
#pragma comment(linker, "/merge:.data=.mydata")

#pragma bss_seg(".payload")
char payload_data[BINARY_PAYLOAD_SIZE];