#pragma once

#define BINARY_PAYLOAD_SIZE 0x0B500000

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
#include <sstream>
#include <string>

#include <gsl/gsl>
#include <MinHook.h>

#include <asmjit/core/jitruntime.h>
#include <asmjit/x86/x86assembler.h>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ntdll.lib")

#include "resource.hpp"

using namespace std::literals;