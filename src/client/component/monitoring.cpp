#include <std_include.hpp>
#if 1
#include "loader/component_loader.hpp"
#include <utils/hook.hpp>
#include "game/game.hpp"

#include <utils/string.hpp>

#include "scheduler.hpp"

namespace monitoring
{
	game::cvar_t* cg_drawDisconnect;
	game::cvar_t* cg_drawWeaponSelect;
	game::cvar_t* cg_drawFPS;
	game::cvar_t* cg_lagometer;
	game::cvar_t* cg_chatHeight;
	game::cvar_t* con_boldgamemessagetime;

	utils::hook::detour CG_DrawWeaponSelect_hook;
		
	static void CG_DrawDisconnect_stub()
	{
		if (!cg_drawDisconnect->integer)
			return;
		utils::hook::invoke<void>(ABSOLUTE_CGAME_MP(0x30015450));
	}

	static void CG_DrawWeaponSelect_stub()
	{
		if (!cg_drawWeaponSelect->integer)
			return;
		CG_DrawWeaponSelect_hook.invoke();
	}

#if 0
#define ping ((int*)0x0143b148)

	void cg_draw_ping()
	{
		if (cg_drawPing->integer > 0 && *game::cls_state == game::CA_ACTIVE)
		{
			int font = 1;
			auto* const ping_string = utils::string::va("Ping: %i", *ping);
			const auto scale = 0.25f;
			float ping_color[4] = { 1.0f, 1.0f, 1.0f, 0.65f };
			int x = 200;
			int y = 20;

			game::SCR_DrawString(x, y, font, scale, ping_color, ping_string, NULL, NULL, NULL);
		}
	}
#endif

	void ready_hook_cgame_mp()
	{
		CG_DrawWeaponSelect_hook.create(ABSOLUTE_CGAME_MP(0x30037790), CG_DrawWeaponSelect_stub);

		utils::hook::jump(ABSOLUTE_CGAME_MP(0x300159CC), CG_DrawDisconnect_stub);
		utils::hook::jump(ABSOLUTE_CGAME_MP(0x300159D4), CG_DrawDisconnect_stub);
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi() || game::environment::is_sp())
			{
				return;
			}

			cg_drawFPS = game::Cvar_Get("cg_drawFPS", "0", CVAR_ARCHIVE);
			cg_drawWeaponSelect = game::Cvar_Get("cg_drawWeaponSelect", "1", CVAR_ARCHIVE);
			
			if (game::environment::is_mp())
			{
				//cg_drawPing = game::Cvar_Get("cg_drawPing", "0", CVAR_ARCHIVE);
				//scheduler::loop(cg_draw_ping, scheduler::pipeline::renderer);

				cg_drawDisconnect = game::Cvar_Get("cg_drawDisconnect", "1", CVAR_ARCHIVE);
				cg_chatHeight = game::Cvar_Get("cg_chatHeight", "8", CVAR_ARCHIVE);
				con_boldgamemessagetime = game::Cvar_Get("con_boldgamemessagetime", "8", CVAR_ARCHIVE);
				cg_lagometer = game::Cvar_Get("cg_lagometer", "0", CVAR_ARCHIVE);
			}
		}
	};
}

REGISTER_COMPONENT(monitoring::component)
#endif