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
	LPSTR WINAPI nt_Test_stub()
	{
		auto* orig = static_cast<decltype(GetCommandLineA)*>(nt_Test_hook.get_original());
		auto ret = orig();

		//MessageBoxA(nullptr, ret, "cod-mod", MB_ICONINFORMATION);
		
		std::string pathStr = ret;
		pathStr.erase(pathStr.begin());
		pathStr.erase(pathStr.size() - 2);
		
		if (!strcmp(PathFindFileNameA(pathStr.c_str()), "cod-mod.exe"))
		{
			std::filesystem::path pathFs = pathStr;
			
			auto binary = game::environment::get_binary();
			pathFs.replace_filename(binary);
			pathStr = pathFs.string();
			
			pathStr.insert(pathStr.begin(), '\"');
			pathStr.insert(pathStr.end(), '\"');
			
			strncpy(ret, pathStr.c_str(), strlen(ret) - 1);

			//MessageBoxA(nullptr, ret, "cod-mod", MB_ICONINFORMATION);
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

			nt_GetModuleFileNameA_hook.create(kernel32.get_proc<DWORD(WINAPI*)(HMODULE, LPSTR, DWORD)>("GetModuleFileNameA"), nt_GetModuleFileNameA_stub);
			nt_LoadLibraryA_hook.create(kernel32.get_proc<HMODULE(WINAPI*)(LPCSTR)>("LoadLibraryA"), nt_LoadLibraryA_stub);



			nt_Test_hook.create(kernel32.get_proc<LPSTR(WINAPI*)()>("GetCommandLineA"), nt_Test_stub);




		}
	};
}

REGISTER_COMPONENT(game_module::component)