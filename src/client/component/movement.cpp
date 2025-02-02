#include <std_include.hpp>
#include <utils/hook.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

namespace movement
{
	void Cmd_LookBack()
	{
		game::viewangles[YAW] += 180;
	}
	
	float stockCgZoomSensitivity()
	{
		float* fov_visible_percentage = (float*)ABSOLUTE_CGAME_MP(0x3020958c);
		return *game::fov_visible / *game::cg_fov_cvar_value; // See 30032fe8
	}

	float scaledCgZoomSensitivity()
	{





		return stockCgZoomSensitivity() * 10;





	}

	void cg_zoomSensitivity_scale()
	{
		// See 30032e20
		if (*game::ads_progress == 1)
		{
			*game::cg_zoomSensitivity = scaledCgZoomSensitivity();
		}
		else if (*game::ads_progress != 0)
		{
			auto unknown = (bool*)ABSOLUTE_CGAME_MP(0x30209458); // True when zoomed out before max in, = ads canceled?

			if (*unknown)
				*game::cg_zoomSensitivity = scaledCgZoomSensitivity();
			else
				*game::cg_zoomSensitivity = stockCgZoomSensitivity();
		}
		else
		{
			*game::cg_zoomSensitivity = stockCgZoomSensitivity();
		}

		/*auto unknown = (bool*)ABSOLUTE_CGAME_MP(0x30209458);
		std::ostringstream oss;
		oss << "##### unknown: " << *unknown << ", ads_progress: "<< *game::ads_progress << "\n";
		std::string str = oss.str();
		OutputDebugString(str.c_str());*/
	}
	
	const auto cg_zoomSensitivity_calculation_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.fstp(st0);
			a.call(cg_zoomSensitivity_scale);
			a.ret();
		});







	void ready_hook_cgame_mp()
	{
		//utils::hook::jump(ABSOLUTE_CGAME_MP(0x30032fe8), cg_zoomSensitivity_calculation_stub);
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				return;
			}

			game::Cmd_AddCommand("lookback", Cmd_LookBack);
		}
	};
}

REGISTER_COMPONENT(movement::component)