#pragma once

namespace movement
{
	extern game::cvar_t* sensitivity_adsScaleEnable;
	extern game::cvar_t* sensitivity_adsScale;
	extern game::cvar_t* sensitivity_adsScaleSniperEnable;
	extern game::cvar_t* sensitivity_adsScaleSniper;
	
	void ready_hook_cgame_mp();
}