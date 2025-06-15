#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "sniper_scopes.hpp"
#include "widescreen.hpp"

#include <utils/hook.hpp>
#include <cstring>

namespace scope
{
	game::cvar_t* cg_sniper_scope;

	static void CG_DrawWeapReticle_Stub(float x, float y, float w, float h, float s1, float t1, float s2, float t2, game::qhandle_t hShader)
	{
		float screenW = static_cast<float>(*game::cg_refdef_width);
		float screenH = static_cast<float>(*game::cg_refdef_height);
//		float screenAspect = screenW / screenH;
//		float baseAspect = 1.3333334f;
//		float baseWidth = screenH * baseAspect;
//		float offset =screenW - baseWidth;

		auto BlackShader = (game::qhandle_t)(*game::cgame_mp::syscall)(game::CG_R_REGISTERSHADERNOMIP, "black", 0);
		float background_color[4] = { 0, 0, 0, 1.0f };
		(*game::cgame_mp::syscall)(game::CG_R_SETCOLOR, background_color);

//		if (widescreen::cg_fixedAspect->integer && screenAspect > baseAspect)
//		{
//			game::trap_R_stretchpic(-offset, 0, offset, screenH, s1, t1, s2, t2, BlackShader);
//			game::trap_R_stretchpic(screenW, 0, offset, screenH, s1, t1, s2, t2, BlackShader);
//		}

		if (!cg_sniper_scope->integer)
		{
			utils::hook::invoke<void>(ABSOLUTE_CGAME_MP(0x300310f0), x, y, w, h, s1, t1, s2, t2, hShader);
			return;
		}

		float x1 = x + (w / 2.0f);
		float y1 = y + (h / 2.0f);

		game::trap_R_stretchpic(0, 0, x1, screenH, s1, t1, s2, t2, BlackShader);
		game::trap_R_stretchpic(x1 + w, 0, screenW - (x1 + w), screenH, s1, t1, s2, t2, BlackShader);
		game::trap_R_stretchpic(x1, 0, w, y1, s1, t1, s2, t2, BlackShader);
		game::trap_R_stretchpic(x1, y1 + h, w, screenH - (y1 + h), s1, t1, s2, t2, BlackShader);

		char* weaponData = *(char**)game::weaponbase;
		if (!weaponData)
			return;

		game::qhandle_t scopeShader = 0;
		char* adsShader = *(char**)(weaponData + 0x89 * 4);
		//int* scopeTypeptr = (int*)(weaponData + 0x8A * 4);
		//int  scopeType = *scopeTypeptr;


		if (adsShader && strcmp(adsShader, "ui/assets/reticle_circle_quarter") == 0)
		{
			game::Cvar_Set(cg_sniper_scope->name, "0");
			return;
			// I want to have a method to detect if these actually exist.

			/*
			if (scopeType == 2)  // fg42
				scopeShader = (game::qhandle_t)(*game::cgame_mp::syscall)(game::CG_R_REGISTERSHADERNOMIP, "scope_overlay_german", 0);

			else if (scopeType == 3)  // Springfield
				scopeShader = (game::qhandle_t)(*game::cgame_mp::syscall)(game::CG_R_REGISTERSHADERNOMIP, "scope_overlay_american", 0);

			else if (scopeType == 4)  // g43
				scopeShader = (game::qhandle_t)(*game::cgame_mp::syscall)(game::CG_R_REGISTERSHADERNOMIP, "scope_overlay_russian", 0);

			else
				scopeShader = (game::qhandle_t)(*game::cgame_mp::syscall)(game::CG_R_REGISTERSHADERNOMIP, "scope_overlay_britain", 0);
				*/
		}

		else if (adsShader)
			scopeShader = (game::qhandle_t)(*game::cgame_mp::syscall)(game::CG_R_REGISTERSHADERNOMIP, adsShader, 0);

		else
			return;

		game::trap_R_stretchpic(x1, y1, w, h, s1, t1, s2, t2, scopeShader);

	}

	static void CG_DrawReticleParts_Stub(float x, float y, float w, float h, float s1, float t1, float s2, float t2, game::qhandle_t hShader)
	{
		if (!cg_sniper_scope->integer)
		{
			utils::hook::invoke<void>(ABSOLUTE_CGAME_MP(0x300310f0), x, y, w, h, s1, t1, s2, t2, hShader);
			return;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			cg_sniper_scope = game::Cvar_Get("cg_sniper_scope", "0", game::CVAR_ARCHIVE);
		}

		void post_cgame() override
		{
			utils::hook::call(ABSOLUTE_CGAME_MP(0x30016114), CG_DrawWeapReticle_Stub);				//top left quadrant - used to change scope
			utils::hook::call(ABSOLUTE_CGAME_MP(0x30016134), CG_DrawReticleParts_Stub);				// quadrant 2
			utils::hook::call(ABSOLUTE_CGAME_MP(0x30016153), CG_DrawReticleParts_Stub);				// quadrant 3
			utils::hook::call(ABSOLUTE_CGAME_MP(0x30016173), CG_DrawReticleParts_Stub);				// quadrant 4
			utils::hook::call(ABSOLUTE_CGAME_MP(0x300163ad), CG_DrawReticleParts_Stub);				// fg42 vertical
			utils::hook::call(ABSOLUTE_CGAME_MP(0x30016406), CG_DrawReticleParts_Stub);				// fg42 left
			utils::hook::call(ABSOLUTE_CGAME_MP(0x30016494), CG_DrawReticleParts_Stub);				// springfield vertical
			utils::hook::call(ABSOLUTE_CGAME_MP(0x3001654f), CG_DrawReticleParts_Stub);				// G43 vertical
			utils::hook::call(ABSOLUTE_CGAME_MP(0x300165a9), CG_DrawReticleParts_Stub);				// G43 left horizontal
			utils::hook::call(ABSOLUTE_CGAME_MP(0x300165df), CG_DrawReticleParts_Stub);				// fg24 & G43 right, springfield horizontal
		}
	};
}

REGISTER_COMPONENT(scope::component)