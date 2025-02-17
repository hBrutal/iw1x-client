#include <std_include.hpp>
#if 1
#include "loader/component_loader.hpp"
#include <utils/hook.hpp>
#include "game/game.hpp"

#include "view.hpp"

namespace view
{
	game::cvar_t* cg_fov;
	game::cvar_t* cg_fovScaleEnable;
	game::cvar_t* cg_fovScale;
	
	static float getScaledFOV(float fov)
	{
#if 0
		std::ostringstream oss;
		oss << "####### value: " << fov << "\n";
		std::string str = oss.str();
		OutputDebugString(str.c_str());
#endif
		
		int* flag = (int*)ABSOLUTE_CGAME_MP(0x302071dc); // Might be cg.snap->ps.eFlags
		if (*flag & EF_MG42_ACTIVE)
			return 55;

		if (cg_fovScaleEnable->integer)
			return fov * cg_fovScale->value;

		return fov;
	}
	
	static __declspec(naked) void CG_CalcFov_return_stub_naked()
	{
		__asm
		{
			sub esp, 4;
			fstp dword ptr[esp];			
			push dword ptr[esp];
			call getScaledFOV;
			add esp, 4;

			fstp dword ptr[esp];
			fld dword ptr[esp];
			add esp, 4;

			pop ecx;
			ret;
		}
	}

	void ready_hook_cgame_mp()
	{
		utils::hook::jump(ABSOLUTE_CGAME_MP(0x30032f2a), CG_CalcFov_return_stub_naked);
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			cg_fov = game::Cvar_Get("cg_fov", "80", CVAR_ARCHIVE);
			cg_fovScaleEnable = game::Cvar_Get("cg_fovScaleEnable", "0", CVAR_ARCHIVE);
			cg_fovScale = game::Cvar_Get("cg_fovScale", "1", CVAR_ARCHIVE);
		}
	};
}

REGISTER_COMPONENT(view::component)
#endif