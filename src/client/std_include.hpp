#pragma once

#define BINARY_PAYLOAD_SIZE 0x0B500000

#pragma warning(push)
#pragma warning(disable: 4100)

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

// min and max is required by gdi, therefore NOMINMAX won't work
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <cassert>
#include <chrono>
#include <filesystem>
#include <queue>
#include <sstream>
#include <string>

#include <gsl/gsl>
#include <MinHook.h>

#include <asmjit/core/jitruntime.h>
#include <asmjit/x86/x86assembler.h>

#pragma warning(pop)
#pragma warning(disable: 4100)

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ntdll.lib")

using namespace std::literals;