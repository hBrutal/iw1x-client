#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

#define MAX_HOSTNAME_LENGTH 1024
#define MAX_MAPNAME_LENGTH 18

namespace fixes
{
	utils::hook::detour hook_UI_StartServerRefresh;
	utils::hook::detour hook_CL_Disconnect;

	uintptr_t pfield_charevent_return = 0x40CB77;
	uintptr_t pfield_charevent_continue = 0x40CB23;
	static __declspec(naked) void Field_CharEvent_ignore_console_char_Stub()
	{
		// See https://github.com/xtnded/codextended-client/blob/45af251518a390ab08b1c8713a6a1544b70114a1/cl_input.cpp#L77
		__asm
		{
			cmp ebx, 0x20;
			jge check;
			jmp pfield_charevent_return;

		check:
			cmp ebx, 0x7E;
			jl checked;
			jmp pfield_charevent_return;

		checked:
			jmp pfield_charevent_continue;
		}
	}

	static char* CL_SetServerInfo_hostname_strncpy_Stub(char* dest, const char* src, int destsize)
	{
		char hostname[MAX_HOSTNAME_LENGTH] = {};

		strncpy_s(hostname, sizeof(hostname), utils::string::clean(src, false).c_str(), _TRUNCATE);

		while (
			hostname[0] != '\0' && (
				isspace((unsigned char)hostname[0]) ||
				hostname[0] == '!' ||
				hostname[0] == '\'' ||
				!isprint((unsigned char)hostname[0])
				)
			) 
		{
			memmove(hostname, hostname + 1, strlen(hostname));
		}

		if (hostname[0] == '\0')
		{
			std::string temp = utils::string::clean(src, true);
			if (temp.empty())
				strncpy_s(hostname, sizeof(hostname), "Unnamed Server", _TRUNCATE);
			else
				strncpy_s(hostname, sizeof(hostname), temp.c_str(), _TRUNCATE);
		}

		strncpy_s(dest, destsize, hostname, _TRUNCATE);

		return dest;
	}


	static char* CL_SetServerInfo_mapname_strncpy_Stub(char* dest, const char* src, int destsize)
	{
		strncpy_s(dest, destsize, src, _TRUNCATE);

		for (int i = 0; dest[i]; i++)
			dest[i] = static_cast<char>(tolower(static_cast<unsigned char>(dest[i])));

		if (strncmp(dest, "mp_", 3) == 0 ||
			strncmp(dest, "jm_", 3) == 0 ||
			strncmp(dest, "gg_", 3) == 0 ||
			strncmp(dest, "pv_", 3) == 0 ||
			strncmp(dest, "xp_", 3) == 0 ||
			strncmp(dest, "zh_", 3) == 0)
		{
			memmove(dest, dest + 3, strlen(dest + 3) + 1);
		}

		if (dest[0])
			dest[0] = static_cast<char>(toupper(static_cast<unsigned char>(dest[0])));

		if (strlen(dest) >= MAX_MAPNAME_LENGTH)
			dest[MAX_MAPNAME_LENGTH - 1] = '\0';

		return dest;
	}


	static void CL_Disconnect_Stub(game::qboolean showMainMenu)
	{
		game::Cvar_Set("timescale", "1");
		hook_CL_Disconnect.invoke(showMainMenu);
	}

	static void UI_StartServerRefresh_Stub(game::qboolean full)
	{
		if (*game::refreshActive)
			return;

		hook_UI_StartServerRefresh.invoke(full);
	}
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			/*
			Fixes from 1.1x
			See https://github.com/xtnded/codextended-client/blob/45af251518a390ab08b1c8713a6a1544b70114a1/fixes.cpp
			*/
			utils::hook::nop(0x42D122, 5); // Call Com_AppendCDKey. (fixes the invalid cdkey with fs_game)
			utils::hook::nop(0x40BC18, 5); // Fixes spam with "MAX_PACKET_USERCMDS" if you have 1000 fps.
			utils::hook::nop(0x43BA04, 5); // Removing second "Need Paks:" because it's useless one is enough.
			utils::hook::nop(0x40F6DA, 5); // Call CL_RequestMotd. (crash upon startup net not loaded and socket being sent or smth)
			utils::hook::nop(0x4117B6, 5); // Call CL_RequestMotd. (crash upon startup net not loaded and socket being sent or smth)

			utils::hook::jump(0x40CB1E, Field_CharEvent_ignore_console_char_Stub); // Prevent inserting the char of the console key in the text field.

			utils::hook::call(0x412A2C, CL_SetServerInfo_hostname_strncpy_Stub); // Tidy up server name by removing SOH characters.
			utils::hook::call(0x412A48, CL_SetServerInfo_mapname_strncpy_Stub); // Tidy up map names by removing prefixes.

			hook_CL_Disconnect.create(0x0040EF90, CL_Disconnect_Stub); // Prevent timescale remaining modified after leaving a server/demo.
		}
		void post_cgame() override
		{
			utils::hook::nop(ABSOLUTE_CGAME_MP(0x3002CC29), 6); // Voice chat not found error, leftover from quake3.
			utils::hook::nop(ABSOLUTE_CGAME_MP(0x300217BA), 5); // Railcore shader not found warning, leftover from quake3.
		}
		void post_ui_mp() override
		{
			utils::hook::set(ABSOLUTE_UI_MP(0x40036ABC), &"EXE_MONTH_ABV_JUNE"); // Fix for server browser displaying EXE_MONTH_ABV_JUN
			hook_UI_StartServerRefresh.create(ABSOLUTE_UI_MP(0x4000EA90), UI_StartServerRefresh_Stub); // Prevent displaying servers twice (occurs if double click Refresh List)
		}
	};
}

REGISTER_COMPONENT(fixes::component)
