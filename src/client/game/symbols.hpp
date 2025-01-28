#pragma once




#define WEAK __declspec(selectany)



namespace game
{
	




	/***************************************************************
	 * Functions
	 **************************************************************/
	
	//WEAK symbol<Font_s* (const char* font)> trap_R_RegisterFont{ 0x0, CGAME_OFF(0x301ad770) };
	//WEAK symbol<void(const char* message)> CG_ServerCommand{ 0x0, 0x3002e0d0 };
	




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