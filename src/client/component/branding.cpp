#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "scheduler.hpp"

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
				const auto x = 2;
				const auto y = 10;
				const auto font = 1;
				const auto scale = 0.21f;
				float color[4] = { 0.28f, 0.01f, 1.f, 0.75f };
				const auto* text = MOD_NAME;
				
				game::SCR_DrawString(x, y, font, scale, color, text, NULL, NULL, NULL);

			}, scheduler::pipeline::renderer);
		}
	};
}

REGISTER_COMPONENT(branding::component)