#include "std_include.hpp"

#pragma comment(linker, "/merge:.text=.main")
#pragma comment(linker, "/merge:.rdata=.custom_rdata")
#pragma comment(linker, "/merge:.data=.custom_data")

#pragma bss_seg(".payload")
char payload_data[BINARY_PAYLOAD_SIZE];