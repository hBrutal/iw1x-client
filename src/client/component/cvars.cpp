#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>


namespace cvars
{
	utils::hook::detour hook_CL_Init;
	utils::hook::detour hook_CG_RegisterCvars;
	utils::hook::detour hook_Cvar_command;

	game::cvar_t* r_fullscreen;
	game::cvar_t* cl_bypassMouseInput;
	game::cvar_t* cl_allowDownload;
	game::cvar_t* con_boldgamemessagetime;
	game::cvar_t* com_sv_running;
	game::cvar_t* com_timescale;
	game::cvar_t* sv_cheats;

	namespace vm
	{
		game::vmCvar_t* cg_drawFPS;
		game::vmCvar_t* cg_chatHeight;
		game::vmCvar_t* cg_fov;
		game::vmCvar_t* cg_lagometer;
	}

	static void CL_Init_Stub()
	{
		hook_CL_Init.invoke();

		r_fullscreen = game::Cvar_FindVar("r_fullscreen");
		cl_bypassMouseInput = game::Cvar_FindVar("cl_bypassMouseInput");
		cl_allowDownload = game::Cvar_FindVar("cl_allowDownload");
		con_boldgamemessagetime = game::Cvar_FindVar("con_boldgamemessagetime");
		com_sv_running = game::Cvar_FindVar("sv_running");
		com_timescale = game::Cvar_FindVar("timescale");
		sv_cheats = game::Cvar_FindVar("sv_cheats");

		game::Cvar_Set("version", utils::string::va("COD MP iw1x Build %d %s %s win-x86", __DATE__, __TIME__));
		game::Cvar_Set("shortversion", utils::string::va("%s", VERSIONNUMBER));
		game::Cvar_Set("r_overbrightbits", "0");
		game::Cvar_Set("r_ignorehwgamma", "0");
		game::Cvar_Set("cl_languagewarnings", "0");
		game::Cvar_Set("cl_languagewarningsaserrors", "0");	
	}

	static void CG_RegisterCvars_Stub()
	{
		game::cgame_mp::cvarTable[4].cvarFlags = game::CVAR_ARCHIVE;
		hook_CG_RegisterCvars.invoke();
		/*
		TODO: Hook the loop that calls trap_Cvar_Register, to check for cvarName
		so would not need to:
		- know the index values
		- change cvarFlags before calling CG_RegisterCvars
		*/
		vm::cg_fov = game::cgame_mp::cvarTable[4].vmCvar;
		vm::cg_drawFPS = game::cgame_mp::cvarTable[11].vmCvar;
		vm::cg_lagometer = game::cgame_mp::cvarTable[47].vmCvar;
		vm::cg_chatHeight = game::cgame_mp::cvarTable[83].vmCvar;
	}

	boolean Cvar_Command(void) 
	{
		game::cvar_t* v;

		v = game::Cvar_FindVar(game::Cmd_Argv(0));
		if (!v) 
		{
			return false;
		}

		if (game::Cmd_Argc() == 1) 
		{
			game::Com_Printf("\"%s\" is:\"^5%s^7\" default:\"^5%s^7\"\n", v->name, v->string, v->resetString);
			if (v->latchedString) 
			{
				game::Com_Printf("latched: \"%s\"\n", v->latchedString);
			}
			return true;
		}

		game::Cvar_Set(v->name, game::Cmd_Argv(1));
		return true;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			hook_CL_Init.create(0x00411e60, CL_Init_Stub);
			hook_Cvar_command.create(0x439c40, Cvar_Command);
		}

		void post_cgame() override
		{
			hook_CG_RegisterCvars.create(ABSOLUTE_CGAME_MP(0x300205e0), CG_RegisterCvars_Stub);
		}
	};
}

REGISTER_COMPONENT(cvars::component)