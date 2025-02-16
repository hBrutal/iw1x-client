#include <std_include.hpp>
#if 1
#include "loader/component_loader.hpp"
#include <utils/hook.hpp>
#include "game/game.hpp"

#include "view.hpp"

namespace view
{
	game::cvar_t* cg_fov;
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			cg_fov = game::Cvar_Get("cg_fov", "80", CVAR_ARCHIVE);
		}
	};
}

REGISTER_COMPONENT(view::component)
#endif