#pragma once

namespace widescreen
{
	void ready_hook_ui_mp();
	void ready_hook_cgame_mp();

	extern game::cvar_t* cg_fixedAspect;
	extern game::cvar_t* cg_fixedAspectFOV;
}