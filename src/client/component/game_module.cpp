#include <std_include.hpp>
#include "game/game.hpp"
#include "loader/component_loader.hpp"
#include "game_module.hpp"

#include <utils/hook.hpp>

namespace game_module
{
	utils::hook::detour nt_GetModuleFileName_hook;

	utils::nt::library get_game_module()
	{
		static utils::nt::library game{ HMODULE(0x400000) };
		return game;
	}

	utils::nt::library get_host_module()
	{
		static utils::nt::library host{};
		return host;
	}
	
	// Make the GPU driver enable the proper profile, preventing buffer overrun when CoD calls glGetString(GL_EXTENSIONS)
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
	
	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			get_host_module();
		}

		void post_load() override
		{
			assert(get_host_module() == get_game_module());
			
			const utils::nt::library kernel32("kernel32.dll");
			nt_GetModuleFileName_hook.create(kernel32.get_proc<DWORD(WINAPI*)(HMODULE, LPSTR, DWORD)>("GetModuleFileNameA"), nt_GetModuleFileName_stub);
		}
	};
}

REGISTER_COMPONENT(game_module::component)