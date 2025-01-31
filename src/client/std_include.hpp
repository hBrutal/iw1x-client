#pragma once

#define BINARY_PAYLOAD_SIZE 0x0B500000

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <MsHTML.h>
#include <MsHtmHst.h>
#include <shellapi.h>
#include <csetjmp>
#include <ShlObj.h>
#include <atlbase.h>
#include <dwmapi.h>
#include <GL/gl.h>

// min and max is required by gdi, therefore NOMINMAX won't work
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#define MSG_BOX_INFO(message) MessageBoxA(nullptr, message, "", MB_ICONINFORMATION);
#define MSG_BOX_WARN(message) MessageBoxA(nullptr, message, "", MB_ICONWARNING);
#define MSG_BOX_ERROR(message) MessageBoxA(nullptr, message, "Error", MB_ICONERROR);

#include <cassert>
#include <chrono>
#include <filesystem>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>

#include <gsl/gsl>
#include <MinHook.h>
#include <imgui.h>
#include <backends/imgui_impl_opengl2.h>
#include <backends/imgui_impl_win32.h>

#include <asmjit/core/jitruntime.h>
#include <asmjit/x86/x86assembler.h>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "opengl32.lib")

#include "resource.hpp"

using namespace std::literals;