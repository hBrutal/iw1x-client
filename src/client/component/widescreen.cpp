#include <std_include.hpp>
#if 1
#include "loader/component_loader.hpp"
#include <utils/hook.hpp>
#include "game/game.hpp"

#include <utils/string.hpp>

#include "widescreen.hpp"
/*
=================
This file is for scaling UI / Menus when in widescreen
Done:
scaling for text.

TO DO:
- scaling for menus and hud.
=================
*/

namespace widescreen
{

	game::cvar_t* cg_fixedAspect;
	game::cvar_t* cg_fixedAspectFOV;


	void SCR_AdjustFrom640_Stub(float* x, float* y, float* w, float* h)
	{
		float xscale, offset = 0.0f;
		float yscale = *game::glconfig_vidHeight / 480.0f;

		if (cg_fixedAspect->integer) 
		{
			float baseWidth = (*game::glconfig_vidHeight * 4.0f) / 3.0f;
			xscale = baseWidth / 640.0f;

			if (*game::glconfig_vidWidth * 480 > *game::glconfig_vidHeight * 640) 
			{
				offset = 0.5f * (*game::glconfig_vidWidth - baseWidth);
			}
		}
		else 
		{
			xscale = *game::glconfig_vidWidth / 640.0f;
		}
		if (x) *x *= xscale;
		if (y) *y *= yscale;
		if (w) *w *= xscale;
		if (h) *h *= yscale;

		if (x) *x += offset;
	}
	void ready_hook_ui_mp()
	{
		//Item_RunScript_hook.create(ABSOLUTE_UI_MP(0x400111d0), Item_RunScript_stub);
	}

	void ready_hook_cgame_mp()
	{
		// Use a cvar whitelist for setClientCvar GSC method

	}
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{

			cg_fixedAspect = game::Cvar_Get("cg_fixedAspect", "1", CVAR_ARCHIVE);
			cg_fixedAspectFOV = game::Cvar_Get("cg_fixedAspectFOV", "1", CVAR_ARCHIVE);
			utils::hook::jump(0x416810, SCR_AdjustFrom640_Stub);

		}
	};
}

REGISTER_COMPONENT(widescreen::component)
#endif