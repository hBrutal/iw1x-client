#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "obituary.hpp"
#include "hud.hpp"
#include "scheduler.hpp"

#include <utils/hook.hpp>

namespace obituary
{
	game::cvar_t* cg_Obituary;
	game::cvar_t* cg_WhiteText;
	game::cvar_t* cg_ObituaryColorFilter;
	utils::hook::detour hook_CG_Obituary;

	static int g_CurrentAttacker = -1;
	static int g_CurrentVictim = -1;
	static int g_CurrentPlayer = -1;

	static void strncpy_filter(char* dest, const char* src, int destsize)
	{
		int i = 0, j = 0;
		if (destsize <= 0)
			return;

		while (src[i] != '\0' && j < destsize - 1)
		{
			if (src[i] == '^')
			{
				int caretStart = i;
				while (src[i] == '^')
				{
					i++;
				}
				if (src[i] != '\0' && isdigit((unsigned char)src[i]))
				{
					while (src[i] != '\0' && isdigit((unsigned char)src[i]))
					{
						i++;
					}
					continue;
				}
				else
				{
					for (int k = caretStart; k < i && j < destsize - 1; k++)
					{
						dest[j++] = src[k];
					}
					continue;
				}
			}
			else
			{
				dest[j++] = src[i++];
			}
		}
		dest[j] = '\0';

	}

	static void CG_ObituaryColorFilter(char* dest, const char* src, int destsize)
	{
		if (cg_ObituaryColorFilter->integer)
		{
			strncpy_filter(dest, src, destsize);
			dest[destsize - 1] = 0;
		}
		else
		{
			strncpy_s(dest, destsize, src, _TRUNCATE);
			dest[destsize - 1] = 0;
		}
	}

	static void CG_Obituary_Stub(int ent)
	{
		if (!cg_Obituary->integer) 
			return;

		int attacker = *(int*)(ent + 120);
		int victim = *(int*)(ent + 116);
		int player = *(int*)(game::cg_clientNum);

		g_CurrentAttacker	= attacker;
		g_CurrentVictim		= victim;
		g_CurrentPlayer		= player;
		hook_CG_Obituary.invoke(ent);
	}

	static const void* CL_ConsolePrint_AddLine = (void*)0x408f80;
	void CL_ConsolePrint_AddLine_Stub(int msg_type, int time, int width, int color, const char* msg)
	{
		if (!cg_WhiteText->integer)
		{
			msg_type = 4;
		}

		__asm 
		{
			mov eax, msg

			push color
			push width
			push time
			push msg_type

			call dword ptr[CL_ConsolePrint_AddLine]
			add esp, 16
		}
	}

	__declspec(naked) void CL_ConsolePrint_AddLine_asm()
	{
		__asm
		{
			pushad

			mov esi, eax
			mov eax, [esp + 0x24]
			mov ebx, [esp + 0x28]
			mov ecx, [esp + 0x2C]
			mov edx, [esp + 0x30]

			push esi
			push edx
			push ecx
			push ebx
			push eax

			call CL_ConsolePrint_AddLine_Stub
			add esp, 5 * 4

			popad
			ret
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			cg_Obituary				= game::Cvar_Get("cg_Obituary", "1", game::CVAR_ARCHIVE);
			cg_ObituaryColorFilter	= game::Cvar_Get("cg_ObituaryColorFilter", "0", game::CVAR_ARCHIVE);
			cg_WhiteText			= game::Cvar_Get("cg_WhiteText", "1", game::CVAR_ARCHIVE);

			utils::hook::call(0x401FB5, CL_ConsolePrint_AddLine_asm);
		}

		void post_cgame() override
		{
			utils::hook::call(ABSOLUTE_CGAME_MP(0x3001D861), CG_ObituaryColorFilter);
			utils::hook::call(ABSOLUTE_CGAME_MP(0x3001D8F6), CG_ObituaryColorFilter);
			hook_CG_Obituary.create(ABSOLUTE_CGAME_MP(0x3001D6C0), CG_Obituary_Stub);
		}
	};
}

REGISTER_COMPONENT(obituary::component)