#include <std_include.hpp>
#if 1
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "scheduler.hpp"

namespace branding
{
	game::cvar_t* branding;
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			branding = game::Cvar_Get("branding", "1", CVAR_ARCHIVE);
			
			scheduler::loop([]()
			{
					if (!branding->integer)
						return;
					
					const auto x = 1;
					const auto y = 10;
					const auto fontID = 1;
					const auto scale = 0.21f;
					float color[4] = { 1.f, 1.f, 1.f, 0.80f };
					float color_shadow[4] = { 0.f, 0.f, 0.f, 0.80f };
					const auto* text = MOD_NAME;

					// Draw a drop shadow first				
					game::SCR_DrawString(x + 1, y + 1, fontID, scale, color_shadow, text, NULL, NULL, NULL);				
					game::SCR_DrawString(x, y, fontID, scale, color, text, NULL, NULL, NULL);

			}, scheduler::renderer);
		}
	};
}

REGISTER_COMPONENT(branding::component)
#endif