#include <std_include.hpp>
#if 1
#include "loader/component_loader.hpp"
#include <utils/hook.hpp>
#include "game/game.hpp"

#include <utils/string.hpp>

#include "scheduler.hpp"

namespace ui
{
	game::cvar_t* branding;
	game::cvar_t* cg_drawDisconnect;
	game::cvar_t* cg_drawWeaponSelect;
	game::cvar_t* cg_drawFPS;
	game::cvar_t* cg_lagometer;
	game::cvar_t* cg_chatHeight;
	game::cvar_t* con_boldgamemessagetime;
	game::cvar_t* cg_Obituary;				// enable/disable kills and deaths in killfeed
	game::cvar_t* cg_WhiteText;				// enable/disable white text in killfeed
	game::cvar_t* cg_ObituaryColor;			// enable/disable colour text in killfeed

	utils::hook::detour CG_DrawWeaponSelect_hook;
	utils::hook::detour CG_Obituary_hook;

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

	static void CG_Obituary_stub()
	{
		if (!cg_Obituary->integer)
			return;
		CG_Obituary_hook.invoke();
	}

	static void strncpy_filter_stub(char* dest, const char* src, int destsize)
	{
		int i = 0, j = 0;
		if (destsize <= 0)
			return;

		while (src[i] != '\0' && j < destsize - 1)
		{
			if (src[i] == '^')
			{
				int caretStart = i;
				while (src[i] == '^')
				{
					i++;
				}
				if (src[i] != '\0' && isdigit((unsigned char)src[i]))
				{
					while (src[i] != '\0' && isdigit((unsigned char)src[i]))
					{
						i++;
					}
					continue;
				}
				else
				{
					for (int k = caretStart; k < i && j < destsize - 1; k++)
					{
						dest[j++] = src[k];
					}
					continue;
				}
			}
			else
			{
				dest[j++] = src[i++];
			}
		}
		dest[j] = '\0';

	}
	static void CG_ObituaryColor_stub(char* dest, const char* src, int destsize)
	{
		if (!cg_ObituaryColor->integer)
		{
			strncpy_filter_stub(dest, src, destsize);
			dest[destsize - 1] = 0;
		}
		else
		{
			strncpy_s(dest, destsize, src, _TRUNCATE);
			dest[destsize - 1] = 0;
		}
	}

	static void CG_WhiteText_rename_stub(int player_rename)
	{
		if (!cg_WhiteText->integer)
			return;

		utils::hook::invoke<void>(ABSOLUTE_CGAME_MP(0x3003eb40), player_rename);

	}

	static void __fastcall CG_WhiteText_stub(int message)
	{
		if (!cg_WhiteText->integer)
			return;
		utils::hook::invoke<void>(ABSOLUTE_CGAME_MP(0x300207b0), message);
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
		CG_Obituary_hook.create(ABSOLUTE_CGAME_MP(0x3001D6C0), CG_Obituary_stub);

		utils::hook::jump(ABSOLUTE_CGAME_MP(0x300159CC), CG_DrawDisconnect_stub);
		utils::hook::jump(ABSOLUTE_CGAME_MP(0x300159D4), CG_DrawDisconnect_stub);

		utils::hook::call(ABSOLUTE_CGAME_MP(0x3002fd50), CG_WhiteText_rename_stub);
		utils::hook::call(ABSOLUTE_CGAME_MP(0x3002e195), CG_WhiteText_stub);
		utils::hook::jump(ABSOLUTE_CGAME_MP(0x30038006), CG_WhiteText_stub);

		utils::hook::call(ABSOLUTE_CGAME_MP(0x3001d861), CG_ObituaryColor_stub);	// attacker name
		utils::hook::call(ABSOLUTE_CGAME_MP(0x3001d8f6), CG_ObituaryColor_stub);	// victim name
		utils::hook::call(ABSOLUTE_CGAME_MP(0x3001d951), CG_ObituaryColor_stub);
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			branding = game::Cvar_Get("branding", "1", CVAR_ARCHIVE);
			cg_drawFPS = game::Cvar_Get("cg_drawFPS", "0", CVAR_ARCHIVE);
			cg_drawWeaponSelect = game::Cvar_Get("cg_drawWeaponSelect", "1", CVAR_ARCHIVE);
			cg_drawDisconnect = game::Cvar_Get("cg_drawDisconnect", "1", CVAR_ARCHIVE);
			cg_chatHeight = game::Cvar_Get("cg_chatHeight", "8", CVAR_ARCHIVE);
			con_boldgamemessagetime = game::Cvar_Get("con_boldgamemessagetime", "8", CVAR_ARCHIVE);
			cg_lagometer = game::Cvar_Get("cg_lagometer", "0", CVAR_ARCHIVE);

			cg_Obituary = game::Cvar_Get("cg_Obituary", "1", CVAR_ARCHIVE);
			cg_WhiteText = game::Cvar_Get("cg_WhiteText", "1", CVAR_ARCHIVE);
			cg_ObituaryColor = game::Cvar_Get("cg_ObituaryColor", "1", CVAR_ARCHIVE);
			
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






			//cg_drawPing = game::Cvar_Get("cg_drawPing", "0", CVAR_ARCHIVE);
			//scheduler::loop(cg_draw_ping, scheduler::pipeline::renderer);
		}
	};
}

REGISTER_COMPONENT(ui::component)
#endif