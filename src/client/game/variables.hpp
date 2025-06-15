#pragma once
#define WEAK __declspec(selectany)

namespace game
{
	constexpr auto MAX_STRING_CHARS = 1024;
	constexpr auto CS_SERVERINFO = 0;
	constexpr auto CS_SYSTEMINFO = 1; 
	
	constexpr auto CVAR_ARCHIVE = 1;
	constexpr auto CVAR_LATCH = 32;

	constexpr auto KEYCATCH_CONSOLE = 0x0001;
	constexpr auto KEYCATCH_UI = 0x0002;
	constexpr auto KEYCATCH_MESSAGE = 0x0004;
	constexpr auto KEYCATCH_CGAME = 0x0008;
	constexpr auto EF_MG42_ACTIVE = 0xc000;
	constexpr auto YAW = 1;
	constexpr auto FPS_FRAMES = 32;

	namespace cgame_mp
	{
		WEAK symbol<cvarTable_t> cvarTable{ 0x300749A0, BASE_CGAME_MP };
		WEAK symbol<int (*)(int arg, ...)> syscall{ 0x30074898, BASE_CGAME_MP };
	}

	namespace ui_mp
	{
		WEAK symbol<int (*)(int arg, ...)> syscall{ 0x40036030, BASE_UI_MP };
	}

	WEAK symbol<int> cls_keyCatchers{ 0x0155f2c4 };
	WEAK symbol<HWND> hWnd{ 0x16C35E8 };
	WEAK symbol<int> cmd_argc{ 0x008930f0 };
	WEAK symbol<char*> cmd_argv{ 0x00890bf0 };
	WEAK symbol<float> viewangles{ 0x0143a9a0 };
	WEAK symbol<qboolean> clc_demoplaying{ 0x015ef004 };
	WEAK symbol<vm_t*> cgvm{ 0x01617348 };
	WEAK symbol<qboolean> mouseActive{ 0x8e2520 };
	WEAK symbol<qboolean> mouseInitialized{ 0x8e2524 };
	WEAK symbol<connstate_t> cls_state{ 0x155F2C0 };
	WEAK symbol<qboolean> clc_demorecording{ 0x015ef000 };
	WEAK symbol<cvar_t[]> cvar_indexes{ 0x0163b420 };
	WEAK symbol<netadr_t> clc_serverAddress{ 0x015ce86c };
	WEAK symbol<netadr_t> cls_autoupdateServer{ 0x015ca57c };
	WEAK symbol<int> vidWidth{ 0x16C3AE4 };
	WEAK symbol<int> vidHeight{ 0x16C3AE8 };
	WEAK symbol<int> glconfig_vidWidth{ 0x15CA614 };
	WEAK symbol<int> glconfig_vidHeight{ 0x15CA618 };
	WEAK symbol<qhandle_t> whiteShader{ 0x15ca630 };

	//CGAME_MP
	WEAK symbol<int> cg_clientNum{ 0x301e2144, BASE_CGAME_MP };
	WEAK symbol<int> cg_time{ 0x30207148, BASE_CGAME_MP };
	WEAK symbol<float> cg_refdef_XScale{ 0x301D1FC4, BASE_CGAME_MP };
	WEAK symbol<float> cg_refdef_YScale{ 0x301D1FC8, BASE_CGAME_MP };
	WEAK symbol<float> cg_refdef_X{ 0x3020957c, BASE_CGAME_MP };
	WEAK symbol<float> cg_refdef_Y{ 0x30209580, BASE_CGAME_MP };
	WEAK symbol<float> CG_30060F10{ 0x30060F10, BASE_CGAME_MP };
	WEAK symbol<int> cg_refdef_width{ 0x30209584, BASE_CGAME_MP };
	WEAK symbol<int> cg_refdef_height{ 0x30209588, BASE_CGAME_MP };
	WEAK symbol<int> cg_renderingThirdPerson{ 0x30207158, BASE_CGAME_MP };
	WEAK symbol<float> cg_refdef_vieworg_0{ 0x30209594, BASE_CGAME_MP };
	WEAK symbol<float> cg_refdef_vieworg_1{ 0x30209598, BASE_CGAME_MP };
	WEAK symbol<float> cg_refdef_vieworg_2{ 0x3020959c, BASE_CGAME_MP };

	WEAK symbol<float> cg_velocity_Y{ 0x301E2194, BASE_CGAME_MP };
	WEAK symbol<float> cg_velocity_X{ 0x301E2198, BASE_CGAME_MP };
	WEAK symbol<float> cg_velocity_Z{ 0x301E219C, BASE_CGAME_MP };
	WEAK symbol<float> cg_zoomSensitivity{ 0x3020b5f4, BASE_CGAME_MP };
	WEAK symbol<float> ads_progress{ 0x30207214, BASE_CGAME_MP }; // Between 0 and 1, might be cg.zoomTime
	WEAK symbol<float> fov_visible{ 0x3020958c, BASE_CGAME_MP }; // Not the cg_fov cvar value, might be cg.zoomval
	WEAK symbol<pmove_t*> pm{ 0x3019d570, BASE_CGAME_MP };
	WEAK symbol<weaponInfo_t[]> cg_weapons{ 0x300eef3c, BASE_CGAME_MP };
	WEAK symbol<char*> weaponbase{ 0x30209484, BASE_CGAME_MP };
	WEAK symbol<int> weapon_Info{ 0x30209484, BASE_CGAME_MP };
	WEAK symbol<int> weapon_shader{ 0x301A6AA4, BASE_CGAME_MP };

	WEAK symbol<const char*> game_message_var{ 0x301dfc2c, BASE_CGAME_MP };

	//
	// UI_MP
	WEAK symbol<qboolean> refreshActive{ 0x401EA698, BASE_UI_MP };
	WEAK symbol<int> uiInfo_uiDC_cursorX{ 0x401c3dd4, BASE_UI_MP };
	WEAK symbol<int> uiInfo_uiDC_cursorY{ 0x401c3dd8, BASE_UI_MP };
	//WEAK <vm_t*> uivm{ 0, 0x0161747c };
	//WEAK <int> ping{ 0x41405d };
}
namespace cvars
{
	extern game::cvar_t* r_fullscreen;
	extern game::cvar_t* cl_bypassMouseInput;
	extern game::cvar_t* cl_allowDownload;
	extern game::cvar_t* con_boldgamemessagetime;
	extern game::cvar_t* com_sv_running;
	extern game::cvar_t* com_timescale;
	extern game::cvar_t* sv_cheats;

	namespace vm
	{
		extern game::vmCvar_t* cg_lagometer;
		extern game::vmCvar_t* cg_fov;
		extern game::vmCvar_t* cg_drawFPS;
		extern game::vmCvar_t* cg_chatHeight;
	}
}