#include <std_include.hpp>
#include <utils/hook.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

namespace protection
{
	
	
	std::vector<std::string> writeProtectedCvars =
	{
		"activeAction",
		"cg_norender",
		"cl_allowDownload",
		"cl_avidemo",
		"m_pitch",
		"m_yaw",
		"name",
		"r_showImages",
		"sensitivity",
	};


	void CL_SystemInfoChanged_Cvar_Set(const char* name, const char* value)
	{
		for (const auto& str : writeProtectedCvars)
			if (!_stricmp(str.c_str(), name))
				return;
		game::Cvar_Set(name, value);
	}
	

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi())
				return;
			
			utils::hook::call(SP_OR_MP(0, 0x00415ffe), CL_SystemInfoChanged_Cvar_Set);

		}
	};
}

REGISTER_COMPONENT(protection::component)