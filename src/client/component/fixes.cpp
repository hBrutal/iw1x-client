#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include <utils/hook.hpp>
#include "game/game.hpp"

namespace fixes
{
	utils::hook::detour UI_StartServerRefresh_hook;
	
	// See https://github.com/xtnded/codextended-client/blob/45af251518a390ab08b1c8713a6a1544b70114a1/cl_input.cpp#L77
	auto pfield_charevent_return = 0x40CB77;
	auto pfield_charevent_continue = 0x40CB23;
	const auto Field_CharEvent_ignore_console_char_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			const auto check = a.newLabel();
			const auto checked = a.newLabel();

			a.cmp(ebx, 0x20);
			a.jge(check);
			a.jmp(pfield_charevent_return);

			a.bind(check);
			a.cmp(ebx, 126);
			a.jl(checked);
			a.jmp(pfield_charevent_return);

			a.bind(checked);
			a.jmp(pfield_charevent_continue);
		});
	
	void UI_StartServerRefresh_stub(game::qboolean full)
	{
		if (*game::refreshActive)
			return;
		UI_StartServerRefresh_hook.invoke(full);
	}

	char* Q_CleanStr_keep_colors(char* string)
	{
		char* d;
		char* s;
		int c;
		s = string;
		d = string;
		while ((c = *s) != 0)
		{
			if (c >= 0x20 && c <= 0x7E)
			{
#pragma warning(push)
#pragma warning(disable: 4244)
				* d++ = c;
#pragma warning(pop)
			}
			s++;
		}
		*d = '\0';
		return string;
	}
	char* CL_SetServerInfo_hostname_strncpy_stub(char* dest, const char* src, int destsize)
	{
		char hostname[MAX_STRING_CHARS];
		strncpy_s(hostname, sizeof(hostname), src, _TRUNCATE);
		Q_CleanStr_keep_colors(hostname);
#pragma warning(push)
#pragma warning(disable: 4996)
		strncpy(dest, hostname, destsize); // destsize is already max-1 (=31), so not using _TRUNCATE not to lose a char
#pragma warning(pop)
		return dest;
	}
	
	void ready_hook_ui_mp()
	{

		/*std::ostringstream oss;
		oss << "####### " << std::hex << ABSOLUTE_UI_MP(0x4000ea90) << "\n";
		std::string str = oss.str();
		OutputDebugString(str.c_str());*/
		


		// Prevents displaying servers twice (if double click Refresh List)
		UI_StartServerRefresh_hook.create(ABSOLUTE_UI_MP(0x4000ea90), UI_StartServerRefresh_stub);
		// Prevents displaying squares in server names
		utils::hook::call(SP_OR_MP(0, 0x412A2C), CL_SetServerInfo_hostname_strncpy_stub);
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			// Prevents inserting the char of the console key in the text field
			utils::hook::jump(SP_OR_MP(0, 0x40CB1E), Field_CharEvent_ignore_console_char_stub);
		}
	};
}

REGISTER_COMPONENT(fixes::component)