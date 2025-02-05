#pragma once

namespace monitoring
{
	extern game::cvar_t* cg_drawFPS;
	extern game::cvar_t* cg_lagometer;
	extern game::cvar_t* cg_drawDisconnect;
	extern game::cvar_t* cg_drawWeaponSelect;
	extern game::cvar_t* cg_chatHeight;
	extern game::cvar_t* con_boldgamemessagetime;

	void ready_hook_cgame_mp();
}