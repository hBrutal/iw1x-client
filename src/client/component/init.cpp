#include <std_include.hpp>
#include <utils/hook.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "security.hpp"
#include "fixes.hpp"
#include "monitoring.hpp"
#include "movement.hpp"

DWORD address_cgame_mp;
DWORD address_ui_mp;

namespace init
{
	utils::hook::detour nt_GetModuleFileNameA_hook;
	utils::hook::detour nt_GetModuleFileNameW_hook;
	
	void hook_dll_cgame_mp()
	{
		security::ready_hook_cgame_mp();
		monitoring::ready_hook_cgame_mp();
		movement::ready_hook_cgame_mp();
	}

	void hook_dll_ui_mp()
	{
		security::ready_hook_ui_mp();
		fixes::ready_hook_ui_mp();
	}
	
	HMODULE WINAPI LoadLibraryA_stub(LPCSTR lpLibFileName)
	{
		auto ret = LoadLibraryA(lpLibFileName);
		auto hModule_address = (DWORD)GetModuleHandleA(lpLibFileName);

		if (lpLibFileName != NULL)
		{
			auto fileName = PathFindFileNameA(lpLibFileName);
			if (!strcmp(fileName, "cgame_mp_x86.dll"))
			{
				address_cgame_mp = hModule_address;
				hook_dll_cgame_mp();
			}
			else if (!strcmp(fileName, "ui_mp_x86.dll"))
			{
				address_ui_mp = hModule_address;
				hook_dll_ui_mp();
			}
		}

		return ret;
	}

	/*
	* Return original client filename, so GPU driver knows what game it is,
	* so if it has a profile for it, it will get enabled
	* (this prevents buffer overrun when glGetString(GL_EXTENSIONS) gets called)
	*/
	// For AMD and Intel HD Graphics
	DWORD WINAPI nt_GetModuleFileNameA_stub(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
	{
		auto* orig = static_cast<decltype(GetModuleFileNameA)*>(nt_GetModuleFileNameA_hook.get_original());
		auto ret = orig(hModule, lpFilename, nSize);
		
		if (!strcmp(PathFindFileNameA(lpFilename), "cod-mod.exe"))
		{
			std::filesystem::path path = lpFilename;
			auto binary = game::environment::get_client_filename();
			path.replace_filename(binary);
			std::string pathStr = path.string();
			std::copy(pathStr.begin(), pathStr.end(), lpFilename);
			lpFilename[pathStr.size()] = '\0';
		}

		return ret;
	}
	// For Nvidia
	DWORD WINAPI nt_GetModuleFileNameW_stub(HMODULE hModule, LPWSTR lpFilename, DWORD nSize)
	{
		auto* orig = static_cast<decltype(GetModuleFileNameW)*>(nt_GetModuleFileNameW_hook.get_original());
		auto ret = orig(hModule, lpFilename, nSize);

		int required_size = WideCharToMultiByte(CP_UTF8, 0, lpFilename, -1, nullptr, 0, nullptr, nullptr);
		std::string pathStr(required_size - 1, '\0');
		WideCharToMultiByte(CP_UTF8, 0, lpFilename, -1, pathStr.data(), required_size, nullptr, nullptr);

		if (!strcmp(PathFindFileNameA(pathStr.c_str()), "cod-mod.exe"))
		{
			std::filesystem::path pathFs = pathStr;

			auto client_filename = game::environment::get_client_filename();
			pathFs.replace_filename(client_filename);
			pathStr = pathFs.string();

			required_size = MultiByteToWideChar(CP_UTF8, 0, pathStr.c_str(), -1, nullptr, 0);
			MultiByteToWideChar(CP_UTF8, 0, pathStr.c_str(), -1, lpFilename, required_size);
		}

		return ret;
	}
	
	void Cmd_Test()
	{
#if 0
		std::ostringstream oss;
		oss << "###### " << *game::cls_keyCatchers << "\n";
		std::string str = oss.str();
		OutputDebugString(str.c_str());
#endif
	}
	
	class component final : public component_interface
	{
	public:
		void* load_import(const std::string&, const std::string& function) override
		{
			if (function == "LoadLibraryA")
			{
				return LoadLibraryA_stub;
			}
			return nullptr;
		}

		void post_load() override
		{
			const utils::nt::library kernel32("kernel32.dll");
			
			nt_GetModuleFileNameA_hook.create(kernel32.get_proc<DWORD(WINAPI*)(HMODULE, LPSTR, DWORD)>("GetModuleFileNameA"), nt_GetModuleFileNameA_stub);
			nt_GetModuleFileNameW_hook.create(kernel32.get_proc<DWORD(WINAPI*)(HMODULE, LPWSTR, DWORD)>("GetModuleFileNameW"), nt_GetModuleFileNameW_stub);
		}
		
		void post_unpack() override
		{
			game::Cmd_AddCommand("test", Cmd_Test);
		}
	};
}

REGISTER_COMPONENT(init::component)