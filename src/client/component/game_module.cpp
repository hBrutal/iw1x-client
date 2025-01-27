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
	
	utils::hook::detour nt_GetModuleFileName_hook;
	utils::hook::detour nt_LoadLibrary_hook;

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

	// Return client filename so GPU driver enables its profile, preventing buffer overrun when glGetString(GL_EXTENSIONS) gets called
	DWORD WINAPI nt_GetModuleFileName_stub(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
	{
		auto* orig = static_cast<decltype(GetModuleFileNameA)*>(nt_GetModuleFileName_hook.get_original());
		auto ret = orig(hModule, lpFilename, nSize);
		std::string path(lpFilename);
		if (path.find("cod-mod.exe") != std::string::npos)
		{
			size_t lastSlash = path.find_last_of("\\/");
			if (lastSlash != std::string::npos)
			{
				auto binary = game::environment::get_binary();
				path = path.substr(0, lastSlash + 1) + binary;
			}
			strncpy(lpFilename, path.c_str(), nSize - 1);
			lpFilename[nSize - 1] = '\0';
		}
		return ret;
	}

	HMODULE WINAPI nt_LoadLibrary_stub(LPCSTR lpFileName)
	{
		auto* orig = static_cast<decltype(LoadLibraryA)*>(nt_LoadLibrary_hook.get_original());
		auto ret = orig(lpFileName);
		DWORD hModule = (DWORD)GetModuleHandleA(lpFileName);

		if (lpFileName != NULL)
		{
			if (strstr(lpFileName, "cgame_mp_x86"))
			{
				cgame_mp = hModule;
				hook_dll_cg_mp();
			}
		}

		return ret;
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

			nt_GetModuleFileName_hook.create(kernel32.get_proc<DWORD(WINAPI*)(HMODULE, LPSTR, DWORD)>("GetModuleFileNameA"), nt_GetModuleFileName_stub);
			nt_LoadLibrary_hook.create(kernel32.get_proc<HMODULE(WINAPI*)(LPCSTR)>("LoadLibraryA"), nt_LoadLibrary_stub);
		}
	};
}

REGISTER_COMPONENT(game_module::component)