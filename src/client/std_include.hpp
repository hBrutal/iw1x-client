#pragma once

// MP: 0x0B459A00
// SP: 0x079C6A00
#define BINARY_PAYLOAD_SIZE 0x0B500000

/*#pragma warning(push)
#pragma warning(disable: 4100)
#pragma warning(disable: 4127)
#pragma warning(disable: 4244)
#pragma warning(disable: 4458)
#pragma warning(disable: 4702)
#pragma warning(disable: 4996)
#pragma warning(disable: 5054)*/

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <MsHTML.h>
#include <MsHtmHst.h>
#include <ExDisp.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <corecrt_io.h>
#include <fcntl.h>
#include <shellapi.h>
#include <csetjmp>
#include <ShlObj.h>
#include <winternl.h>
#include <VersionHelpers.h>
#include <Psapi.h>
#include <urlmon.h>
#include <atlbase.h>
#include <iphlpapi.h>
#include <wincrypt.h>

// min and max is required by gdi, therefore NOMINMAX won't work
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <atomic>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <map>
#include <mutex>
#include <optional>
#include <queue>
#include <random>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include <gsl/gsl>
#include <MinHook.h>

#include <asmjit/core/jitruntime.h>
#include <asmjit/x86/x86assembler.h>

/*#pragma warning(pop)
#pragma warning(disable: 4100)*/

#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "urlmon.lib" )
#pragma comment(lib, "ws2_32.lib")

using namespace std::literals;