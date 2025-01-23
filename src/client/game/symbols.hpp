#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	/***************************************************************
	 * Functions
	 **************************************************************/

	//WEAK symbol<void(unsigned int id)> AddRefToObject{ 0x1403D7A10, 0x1404326D0 };

	/***************************************************************
	 * Variables
	 **************************************************************/

	//WEAK symbol<int> keyCatchers{ 0x1417CF6E0, 0x1419E1ADC };


	/*namespace sp
	{
		WEAK symbol<gentity_s> g_entities{ 0x143C91600, 0 };

	}

	namespace mp
	{
		WEAK symbol<cg_s> cgArray{ 0, 0x14176EC00 };

		WEAK symbol<gentity_s> g_entities{ 0, 0x14427A0E0 };

		WEAK symbol<int> svs_clientCount{ 0, 0x14647B28C };
		WEAK symbol<client_t> svs_clients{ 0, 0x14647B290 };

		WEAK symbol<std::uint32_t> sv_serverId_value{ 0, 0x144DF9478 };

		WEAK symbol<int> gameTime{ 0, 0x1443F4B6C };
		WEAK symbol<int> serverTime{ 0, 0x14647B280 };

	}*/
}