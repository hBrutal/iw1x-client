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
	const auto Field_CharEvent_IgnoreTilde = utils::hook::assemble([](utils::hook::assembler& a)
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
	
	void ready_hook_ui_mp()
	{
		// Prevents displaying servers twice (if double click Refresh List)
		UI_StartServerRefresh_hook.create(ABSOLUTE_UI_MP(0x4000ea90), UI_StartServerRefresh_stub);
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

			// Prevents the char of the key used to open console from getting written in it
			utils::hook::jump(SP_OR_MP(0, 0x40CB1E), Field_CharEvent_IgnoreTilde);
		}
	};
}

REGISTER_COMPONENT(fixes::component)