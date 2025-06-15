#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "widescreen.hpp"
#include "view.hpp"

#include <utils/hook.hpp>

namespace view
{
	game::cvar_t* cg_fovScaleEnable;
	game::cvar_t* cg_fovScale;
	game::cvar_t* cl_autorecord;

	utils::hook::detour hook_CG_Respawn;
	utils::hook::detour hook_CL_PlayDemo_f;
	utils::hook::detour hook_CL_DemoCompleted;
	
	static float scaledFOV(float fov)
	{
//		if (widescreen::cg_fixedAspect->integer)
//		{
//			// Based on LordHavoc's code for Darkplaces
//			// http://www.quakeworld.nu/forum/topic/53/what-does-your-qw-look-like/page/30
//			const float baseAspect = 0.75f;
//			const float aspect = (float)*game::cg_refdef_width / (float)*game::cg_refdef_height;
//			const float desiredFov = fov;
//
//			fov = atan2(tan(desiredFov * M_PI / 360.0f) * baseAspect * aspect, 1.0f) * 360.0f / M_PI;
//		}

		int* flag = (int*)ABSOLUTE_CGAME_MP(0x302071dc); // Might be cg.snap->ps.eFlags

		if (*flag & game::EF_MG42_ACTIVE)
			return 55;

		if (cg_fovScaleEnable->integer)
			return fov * cg_fovScale->value;

		return fov;
	}
	
	static __declspec(naked) void CG_CalcFov_return_Stub()
	{
		__asm
		{
			sub esp, 0x4;
			fstp dword ptr[esp];			
			push dword ptr[esp];
			call scaledFOV;
			add esp, 0x4;

			fstp dword ptr[esp];
			fld dword ptr[esp];
			add esp, 0x4;

			pop ecx;
			ret;
		}
	}
	
	static void CG_Respawn_Stub()
	{
		hook_CG_Respawn.invoke();

		if (cl_autorecord->integer)
		{
			if (*game::clc_demoplaying)
				return;

			auto pm_flag = *(int*)(*(int*)ABSOLUTE_CGAME_MP(0x301E2160) + 0x18); // Seems to be/related to cg.snap->ps.pm_flags
			if (pm_flag == 0x40000) // Is 0x10000 when following
			{
				if (*game::clc_demorecording)
					game::Cbuf_ExecuteText(game::EXEC_APPEND, "stoprecord; wait 5; record\n");
				
				else
					game::Cbuf_ExecuteText(game::EXEC_APPEND, "record\n");
			}
		}
	}

	static void CL_PlayDemo_f_Stub()
	{
		hook_CL_PlayDemo_f.invoke();
		utils::hook::set(0x163b440, 0x01);	// using this instead of sv_cheats as you couldnt use some cvars in demos.
	}

	static void CL_DemoCompleted_Stub()
	{
		hook_CL_DemoCompleted.invoke();
		utils::hook::set(0x163b440, 0x00);
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			cg_fovScaleEnable = game::Cvar_Get("cg_fovScaleEnable", "0", game::CVAR_ARCHIVE);
			cg_fovScale = game::Cvar_Get("cg_fovScale", "1", game::CVAR_ARCHIVE);
			cl_autorecord = game::Cvar_Get("cl_autorecord", "0", game::CVAR_ARCHIVE);

			hook_CL_PlayDemo_f.create(0x0040eb40, CL_PlayDemo_f_Stub);
			hook_CL_DemoCompleted.create(0x0040e920, CL_DemoCompleted_Stub);
		}

		void post_cgame() override
		{
			utils::hook::jump(ABSOLUTE_CGAME_MP(0x30032f2a), CG_CalcFov_return_Stub);
			hook_CG_Respawn.create(ABSOLUTE_CGAME_MP(0x30028a70), CG_Respawn_Stub);
		}
	};
}

REGISTER_COMPONENT(view::component)