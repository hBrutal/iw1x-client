#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	WEAK symbol<int> keyCatchers{ 0, 0x0155f2c4 };
	WEAK symbol<int> ping{ 0, 0x41405d };
	WEAK symbol<HWND> hWnd{ 0, 0x16C35E8 };
	WEAK symbol<int> cmd_argc{ 0, 0x008930f0 };
	WEAK symbol<char*> cmd_argv{ 0, 0x00890bf0 };
	WEAK symbol<float> viewangles{ 0, 0x0143a9a0 };
	WEAK symbol<float> cg_zoomSensitivity{ 0, 0x3020b5f4, OFFSET_CGAME_MP };
	WEAK symbol<float> ads_progress{ 0, 0x30207214, OFFSET_CGAME_MP }; // Between 0 and 1
	WEAK symbol<float> fov_visible{ 0, 0x3020958c, OFFSET_CGAME_MP }; // Not the cg_fov cvar value
	WEAK symbol<float> cg_fov_cvar_value{ 0, 0x30298c68, OFFSET_CGAME_MP };
	WEAK symbol<pmove_t*> pm{ 0, 0x3019d570, OFFSET_CGAME_MP };
	WEAK symbol<weaponInfo_t[MAX_WEAPONS]> cg_weapons{ 0, 0x300eef3c, OFFSET_CGAME_MP };
	WEAK symbol<qboolean> refreshActive{ 0, 0x401EA698, OFFSET_UI_MP };
	
	
	
	
	
	
	
	
	
	
	//WEAK symbol<connstate_t> cls_state{ 0, 0x155F2C0 };
	//WEAK symbol<vm_t*> cgvm{ 0, 0x01617348 };
	//WEAK symbol<vm_t*> uivm{ 0, 0x0161747c };
}