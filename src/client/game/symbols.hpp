#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	/***************************************************************
	 * Functions
	 **************************************************************/


	WEAK symbol<void()> Com_Frame{ 0, 0x00437f40 };
	WEAK symbol<void(int msec)> SV_Frame{ 0, 0x0045b1d0 };
	WEAK symbol<void(float x, float y, int font, float scale, float* color, const char* text, float spacing, int limit, int flags)> SCR_DrawString{ 0x0, 0x4DF570 };







	
	
	/***************************************************************
	 * Variables
	 **************************************************************/

	//WEAK symbol<int> keyCatchers{0x1417CF6E0, 0x1419E1ADC};
	//WEAK symbol<const char*> glConfig_vendor_string{ 0x0, 0x0050ac9b };







	/*namespace sp
	{
		WEAK symbol<gentity_s> g_entities{0x143C91600, 0};
	}

	namespace mp
	{
		WEAK symbol<cg_s> cgArray{0, 0x14176EC00};
	}*/


}