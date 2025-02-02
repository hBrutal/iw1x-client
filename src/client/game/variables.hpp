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





}