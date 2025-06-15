#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "hud.hpp"
#include "scheduler.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace hud
{
	game::cvar_t* cg_drawDisconnect;
	game::cvar_t* cg_drawWeaponSelect;
	//game::cvar_t* cg_ShowHitmarker;

	utils::hook::detour hook_CG_DrawWeaponSelect;

	//static float damageAlpha = 0.0f;
	//static float damageStartTime = 0.0f;
	//static const float damageFadeDuration = 1.0f;

	static void CG_DrawDisconnect_Stub()
	{
		if (!cg_drawDisconnect->integer)
			return;
		utils::hook::invoke<void>(ABSOLUTE_CGAME_MP(0x30015450));
	}

	static void CG_DrawWeaponSelect_Stub()
	{
		if (!cg_drawWeaponSelect->integer)
			return;
		hook_CG_DrawWeaponSelect.invoke();
	}
	/*
	void Show_DamageFeedback()
	{
		damageAlpha = 1.0f;
		damageStartTime = *game::cg_time / 1000.0f;
	}

	void Draw_DamageFeedback()
	{
		if (damageAlpha <= 0.0f)
			return;

		float currentTime = *game::cg_time / 1000.0f;
		float elapsed = currentTime - damageStartTime;

		if (elapsed >= damageFadeDuration) {
			damageAlpha = 0.0f;
			return;
		}

		float alpha = 1.0f - (elapsed / damageFadeDuration);
		float color[4] = { 1.f, 1.f, 1.f, alpha };
		float width = 24.0f, height = 24.0f;
		float x = (640 - width) * 0.5f;
		float y = (480 - height) * 0.5f;

		(*game::cgame_mp::syscall)(game::CG_R_SETCOLOR, color);
		auto damageShader = (game::qhandle_t)(*game::cgame_mp::syscall)(game::CG_R_REGISTERSHADERNOMIP, "damage_feedback", 0);
		game::CG_DrawPic(x, y, width, height, damageShader);
		(*game::cgame_mp::syscall)(game::CG_R_SETCOLOR, NULL);
	}
	*/
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			cg_drawWeaponSelect		= game::Cvar_Get("cg_drawWeaponSelect", "1", game::CVAR_ARCHIVE);
			cg_drawDisconnect		= game::Cvar_Get("cg_drawDisconnect", "1", game::CVAR_ARCHIVE);
			//cg_ShowHitmarker		= game::Cvar_Get("cg_ShowHitmarker", "0", game::CVAR_ARCHIVE);

			//scheduler::loop(Draw_DamageFeedback, scheduler::pipeline::cgame);
		}

		void post_cgame() override
		{
			utils::hook::jump(ABSOLUTE_CGAME_MP(0x300159CC), CG_DrawDisconnect_Stub);
			utils::hook::jump(ABSOLUTE_CGAME_MP(0x300159D4), CG_DrawDisconnect_Stub);
			hook_CG_DrawWeaponSelect.create(ABSOLUTE_CGAME_MP(0x30037790), CG_DrawWeaponSelect_Stub);
		}
	};
}

REGISTER_COMPONENT(hud::component)