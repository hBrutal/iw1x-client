#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	/***************************************************************
	 * Functions
	 **************************************************************/

	WEAK symbol<void ()> Com_Frame{ 0, 0x00437f40 };
	WEAK symbol<void (int msec)> SV_Frame{ 0, 0x0045b1d0 };
	WEAK symbol<void (float x, float y, int font, float scale, float* color, const char* text, float spacing, int limit, int flags)> SCR_DrawString{ 0x0, 0x4DF570 };
	WEAK symbol<cvar_t* (const char* name)> Cvar_FindVar{ 0, 0x00439280 };
	WEAK symbol<cvar_t* (const char* name, const char* value, int flags)> Cvar_Get{ 0, 0x00439350 };
	WEAK symbol<cvar_t* (const char* name, const char* value)> Cvar_Set{ 0, 0x00439650 };
	WEAK symbol<int ()> Sys_Milliseconds{ 0, 0x004659d0 };
	WEAK symbol<LRESULT CALLBACK (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)> MainWndProc{ 0, 0x466BE0 };
	WEAK symbol<void()> IN_DeactivateMouse{ 0, 0x4616b0 };
	WEAK symbol<void()> IN_ActivateMouse{ 0, 0x461730 };


	


	

	
	
	
	
	/***************************************************************
	 * Variables
	 **************************************************************/

	WEAK symbol<int> keyCatchers{ 0, 0x0155f2c4 };
	WEAK symbol<int> ping{ 0, 0x41405d };
	WEAK symbol<HWND> hWnd{ 0, 0x16C35E8 };





}