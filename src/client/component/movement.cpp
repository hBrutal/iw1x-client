#include <std_include.hpp>
#include <utils/hook.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

namespace movement
{
	void Cmd_LookBack()
	{
		game::viewangles[YAW] += 180;
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				return;
			}

			game::Cmd_AddCommand("lookback", Cmd_LookBack);
		}
	};
}

REGISTER_COMPONENT(movement::component)