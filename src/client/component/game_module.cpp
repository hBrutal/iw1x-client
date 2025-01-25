#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game_module.hpp"

#include <utils/hook.hpp>

namespace game_module
{
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
		}
	};
}

REGISTER_COMPONENT(game_module::component)