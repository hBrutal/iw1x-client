#include <std_include.hpp>
#include "game/game.hpp"
#include "loader/component_loader.hpp"
#include "game_module.hpp"

#include <utils/hook.hpp>

#define cgame_mp_offset(relative) (cgame_mp + (relative - 0x30000000))

namespace game_module
{
	DWORD cgame_mp;
	utils::hook::detour CG_ServerCommand_hook;

	void CG_ServerCommand_stub()
	{
		//MessageBoxA(nullptr, "CG_ServerCommand_stub", "cod-mod", MB_ICONINFORMATION);


		CG_ServerCommand_hook.invoke();
	}

	void hook_dll_cg_mp()
	{
		CG_ServerCommand_hook.create(cgame_mp_offset(0x3002e0d0), CG_ServerCommand_stub);
	}
	
	utils::hook::detour nt_GetModuleFileNameA_hook;
	utils::hook::detour nt_LoadLibraryA_hook;
	
	utils::nt::library get_client_module()
	{
		static utils::nt::library client{ HMODULE(0x400000) };
		return client;
	}

	utils::nt::library get_host_module()
	{
		static utils::nt::library host{};
		return host;
	}

	/*
	* Return client filename so GPU driver enables its profile, preventing buffer overrun when glGetString(GL_EXTENSIONS) gets called
	* Doesn't work for Nvidia driver
	*/
	DWORD WINAPI nt_GetModuleFileNameA_stub(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
	{
		auto* orig = static_cast<decltype(GetModuleFileNameA)*>(nt_GetModuleFileNameA_hook.get_original());
		auto ret = orig(hModule, lpFilename, nSize);
		
		if (!strcmp(PathFindFileNameA(lpFilename), "cod-mod.exe"))
		{
			std::filesystem::path path = lpFilename;
			auto binary = game::environment::get_binary();
			path.replace_filename(binary);
			std::string pathStr = path.string();
			strncpy(lpFilename, pathStr.c_str(), nSize - 1);
		}

		return ret;
	}

	HMODULE WINAPI nt_LoadLibraryA_stub(LPCSTR lpLibFileName)
	{
		auto* orig = static_cast<decltype(LoadLibraryA)*>(nt_LoadLibraryA_hook.get_original());
		auto ret = orig(lpLibFileName);
		auto hModule = (DWORD)GetModuleHandleA(lpLibFileName);

		if (lpLibFileName != NULL)
		{
			auto fileName = PathFindFileNameA(lpLibFileName);
			
			if (!strcmp(fileName, "cgame_mp_x86.dll"))
			{
				cgame_mp = hModule;
				hook_dll_cg_mp();
			}
		}

		return ret;
	}














	utils::hook::detour nt_Test_hook;
	NTSTATUS WINAPI nt_Test_stub(const HANDLE handle, const PROCESSINFOCLASS info_class, const PVOID info, const ULONG info_length, const PULONG ret_length)
	{
		auto* orig = static_cast<decltype(NtQueryInformationProcess)*>(nt_Test_hook.get_original());
		const auto status = orig(handle, info_class, info, info_length, ret_length);

		if (NT_SUCCESS(status) && info_class == ProcessBasicInformation)
		{
			auto* pbi = reinterpret_cast<PROCESS_BASIC_INFORMATION*>(info);
			PEB peb;
			if (ReadProcessMemory(handle, reinterpret_cast<LPCVOID>(pbi->PebBaseAddress), &peb, sizeof(PEB), nullptr))
			{
				RTL_USER_PROCESS_PARAMETERS up;
				if (ReadProcessMemory(handle, reinterpret_cast<LPCVOID>(peb.ProcessParameters), &up, sizeof(RTL_USER_PROCESS_PARAMETERS), nullptr))
				{
					UNICODE_STRING imagePathName = up.ImagePathName;
					wchar_t exePath[MAX_PATH];

					if (ReadProcessMemory(handle, reinterpret_cast<LPCVOID>(imagePathName.Buffer), exePath, imagePathName.Length, nullptr))
					{
						exePath[imagePathName.Length / sizeof(wchar_t)] = L'\0';
						std::wstring pathWstr(exePath);
						std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
						std::string pathStr = converter.to_bytes(pathWstr);
						
						if (!strcmp(PathFindFileNameA(pathStr.c_str()), "cod-mod.exe"))
						{
							std::filesystem::path pathFs = pathStr;
							auto binary = game::environment::get_binary();
							pathFs.replace_filename(binary);
							pathStr = pathFs.string();
							
							int bufferSize = MultiByteToWideChar(CP_UTF8, 0, pathStr.c_str(), -1, nullptr, 0);
							LPWSTR wideStr = new wchar_t[bufferSize];
							MultiByteToWideChar(CP_UTF8, 0, pathStr.c_str(), -1, wideStr, bufferSize);

							up.ImagePathName.Buffer = wideStr;
							up.ImagePathName.Length = wcslen(wideStr) * sizeof(wchar_t);
							up.ImagePathName.MaximumLength = MAX_PATH;
							WriteProcessMemory(handle, reinterpret_cast<LPVOID>(peb.ProcessParameters), &up, sizeof(RTL_USER_PROCESS_PARAMETERS), nullptr);
							delete[] wideStr;
						}
					}
				}
			}
		}

		return status;
	}







	
	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			get_host_module();
		}

		void post_load() override
		{
			assert(get_host_module() == get_client_module());
			
			const utils::nt::library kernel32("kernel32.dll");

			nt_GetModuleFileNameA_hook.create(kernel32.get_proc<DWORD(WINAPI*)(HMODULE, LPSTR, DWORD)>("GetModuleFileNameA"), nt_GetModuleFileNameA_stub);
			nt_LoadLibraryA_hook.create(kernel32.get_proc<HMODULE(WINAPI*)(LPCSTR)>("LoadLibraryA"), nt_LoadLibraryA_stub);
			
			
			
			const utils::nt::library ntdll("ntdll.dll");
			nt_Test_hook.create(ntdll.get_proc<void*>("NtQueryInformationProcess"), nt_Test_stub);




		}
	};
}

REGISTER_COMPONENT(game_module::component)