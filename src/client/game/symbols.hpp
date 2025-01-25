#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	/***************************************************************
	 * Functions
	 **************************************************************/

	WEAK symbol<long long (const char* qpath, char** buffer)> FS_ReadFile { 0x0042a8b0 };
	WEAK symbol<void(void* buffer)> FS_FreeFile { 0x0052e7b9 };
	WEAK symbol<void(const char *gameName)> FS_Startup { 0x0042cdb0 };
	WEAK symbol<void(const char *path, const char *dir)> FS_AddGameDirectory{ 0x0042c250 };

	/***************************************************************
	 * Variables
	 **************************************************************/

	//WEAK symbol<int> keyCatchers{0x1417CF6E0, 0x1419E1ADC};

	/*namespace sp
	{
		WEAK symbol<gentity_s> g_entities{0x143C91600, 0};
	}

	namespace mp
	{
		WEAK symbol<cg_s> cgArray{0, 0x14176EC00};
	}*/
}