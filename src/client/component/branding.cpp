#include <std_include.hpp>
#if 1
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
			if (game::environment::is_dedi() || game::environment::is_sp())
			{
				return;
			}

			scheduler::loop([]()
			{
				const auto x = 1;
				const auto y = 12;
				const auto fontID = 1;
				const auto scale = 0.25f;
				float color[4] = { 1.f, 1.f, 1.f, 0.80f };
				float color_shadow[4] = { 0.f, 0.f, 0.f, 0.80f };
				const auto* text = MOD_NAME;

				// Draw a drop shadow first				
				game::SCR_DrawString(x + 1, y + 1, fontID, scale, color_shadow, text, NULL, NULL, NULL);				
				game::SCR_DrawString(x, y, fontID, scale, color, text, NULL, NULL, NULL);

			}, scheduler::pipeline::renderer);
		}
	};
}

REGISTER_COMPONENT(branding::component)
#endif