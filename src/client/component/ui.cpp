#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "ui.hpp"
#include "scheduler.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace ui
{
	game::cvar_t* branding;
	game::cvar_t* cg_Speedometer;
	game::cvar_t* cg_drawFPS_custom;
	game::cvar_t* cg_drawPing;
	game::cvar_t* cg_drawHeadIcon;

	utils::hook::detour hook_CG_DrawUpperRight;
	utils::hook::detour hook_CG_DrawFPS;

	static void draw_branding()
	{
		if (!branding->integer)
			return;

		const auto x = 1;
		const auto y = 10;
		const auto fontID = 1;
		const auto scale = 0.21f;
		float color[4] = { 1.f, 1.f, 1.f, 0.50f };
		float color_shadow[4] = { 0.f, 0.f, 0.f, 0.80f };
		std::string text = std::string(MOD_NAME);

		game::SCR_DrawString(x + 1, y + 1, fontID, scale, color_shadow, text.c_str(), NULL, NULL, NULL);
		game::SCR_DrawString(x, y, fontID, scale, color, text.c_str(), NULL, NULL, NULL);
	}

	static void Draw_Speedometer()
	{
		const auto x = 280;
		const auto y = 420;
		const auto fontID = 4;
		const auto scale = 0.25f;
		float color[4] = { 1.f, 1.f, 1.f, 1.0f };
		float color_shadow[4] = { 0.f, 0.f, 0.f, 0.80f };
		float velY = *game::cg_velocity_Y;
		float velX = *game::cg_velocity_X;
		float velZ = *game::cg_velocity_Z;

		float speed = sqrtf(velX * velX + velY * velY);

		if (cg_Speedometer->integer == 2)
			speed = sqrtf(velX * velX + velY * velY + velZ * velZ);

		std::string text = utils::string::va("Speed: %.2f", speed);

		game::SCR_DrawString(x + 1, y + 1, fontID, scale, color_shadow, text.c_str(), NULL, NULL, NULL);
		game::SCR_DrawString(x, y, fontID, scale, color, text.c_str(), NULL, NULL, NULL);
	}

	static void draw_ping()
	{
		if (*game::clc_demoplaying)
			return;

		int* clSnap_ping = (int*)0x0143b148;
		//int* clSnap_ping = (int*)0x01432978;

		const auto background_x = 475;
		const auto background_y = 0;
		const auto background_width = 85;
		const auto background_height = 15;
		float background_color[4] = { 0, 0, 0, 0.6f };

		(*game::cgame_mp::syscall)(game::CG_R_SETCOLOR, background_color);
		auto shader = (game::qhandle_t)(*game::cgame_mp::syscall)(game::CG_R_REGISTERSHADERNOMIP, "black", 5);
		game::CG_DrawPic(background_x, background_y, background_width, background_height, shader);
		(*game::cgame_mp::syscall)(game::CG_R_SETCOLOR, NULL);

		const auto fontID = 1;
		const auto scale = 0.21f;
		float text_color[4] = { 1, 1, 1, 1 };
		std::string text = utils::string::va("Latency: %i ms", *clSnap_ping);
		game::SCR_DrawString(background_x + 3, background_y + 11, fontID, scale, text_color, text.c_str(), NULL, NULL, NULL);
	}

	static void CG_DrawUpperRight_Stub()
	{
		if (cg_Speedometer->integer)
			Draw_Speedometer();

		if (cg_drawPing->integer)
			draw_ping();

		hook_CG_DrawUpperRight.invoke();
	}

	static void CG_DrawFPS_Stub(float y)
	{
		if (!cg_drawFPS_custom->integer)
		{
			hook_CG_DrawFPS.invoke(y);
			return;
		}

		static int previousTimes[game::FPS_FRAMES];
		static int index;
		int i, total;
		int fps;
		static int previous;
		int t, frameTime;

		t = (int)(*game::cgame_mp::syscall)(game::CG_MILLISECONDS);
		frameTime = t - previous;
		previous = t;

		previousTimes[index % game::FPS_FRAMES] = frameTime;
		index++;
		if (index > game::FPS_FRAMES)
		{
			total = 0;
			for (i = 0; i < game::FPS_FRAMES; i++)
			{
				total += previousTimes[i];
			}
			if (!total)
			{
				total = 1;
			}
			fps = 1000 * game::FPS_FRAMES / total;

			const auto background_x = 570;
			const auto background_y = 0;
			const auto background_width = 50;
			const auto background_height = 15;
			float background_color[4] = { 0, 0, 0, 0.6f };

			(*game::cgame_mp::syscall)(game::CG_R_SETCOLOR, background_color);
			auto shader = (game::qhandle_t)(*game::cgame_mp::syscall)(game::CG_R_REGISTERSHADERNOMIP, "black", 5);
			game::CG_DrawPic(background_x, background_y, background_width, background_height, shader);
			(*game::cgame_mp::syscall)(game::CG_R_SETCOLOR, NULL);

			const auto fontID = 1;
			const auto scale = 0.21f;
			float text_color[4] = { 1, 1, 1, 1 };
			std::string text = utils::string::va("FPS: %i", fps);
			game::SCR_DrawString(background_x + 3, background_y + 11, fontID, scale, text_color, text.c_str(), NULL, NULL, NULL);
		}
	}

	void(*CG_PlayerSprites)();
	void __cdecl CG_PlayerSprites_Stub()
	{
		int* c_ent;
		__asm mov c_ent, eax

		if (cg_drawHeadIcon->integer)
		{
			__asm mov eax, c_ent
			CG_PlayerSprites();
		}
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			branding				= game::Cvar_Get("branding", "1", game::CVAR_ARCHIVE);
			cg_Speedometer			= game::Cvar_Get("cg_Speedometer", "0", game::CVAR_ARCHIVE);
			cg_drawFPS_custom		= game::Cvar_Get("cg_drawFPS_custom", "0", game::CVAR_ARCHIVE);
			cg_drawPing				= game::Cvar_Get("cg_drawPing", "0", game::CVAR_ARCHIVE);
			cg_drawHeadIcon			= game::Cvar_Get("cg_drawHeadIcon", "1", game::CVAR_ARCHIVE);

			scheduler::loop(draw_branding, scheduler::pipeline::renderer);

			utils::hook::set(0x00416b82 + 1, "RECORDING %s: %iKB");
		}

		void post_cgame() override
		{
			CG_PlayerSprites = (void(*)())ABSOLUTE_CGAME_MP(0x300274D0);
			utils::hook::call(ABSOLUTE_CGAME_MP(0x300282DE), CG_PlayerSprites_Stub);
			hook_CG_DrawUpperRight.create(ABSOLUTE_CGAME_MP(0x30015070), CG_DrawUpperRight_Stub);
			hook_CG_DrawFPS.create(ABSOLUTE_CGAME_MP(0x30014a00), CG_DrawFPS_Stub);
		}
	};
}

REGISTER_COMPONENT(ui::component)