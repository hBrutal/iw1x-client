#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>


namespace test
{
	static void SV_BotUserMove_Stub()
	{
		return;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			utils::hook::call(0x45b18f, SV_BotUserMove_Stub);
		}

	};
}

//REGISTER_COMPONENT(test::component)