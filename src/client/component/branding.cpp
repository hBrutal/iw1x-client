#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "scheduler.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace branding
{
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			scheduler::loop([]()
			{
				const auto x = 1;
				const auto y = 10;
				const auto font = 1;
				const auto scale = 0.20;
				float color[4] = { 0.666f, 0.666f, 0.666f, 0.666f };
				const auto* text = "cod-mod";
				
				game::SCR_DrawString(x, y, font, scale, color, text, NULL, NULL, NULL);

			}, scheduler::pipeline::renderer);
		}
	};
}

REGISTER_COMPONENT(branding::component)