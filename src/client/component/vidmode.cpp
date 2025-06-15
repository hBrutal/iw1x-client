#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace vidmode
{
	game::cvar_t* r_mode;
	game::cvar_t* r_customwidth;
	game::cvar_t* r_customheight;
	game::cvar_t* r_customaspect;

	bool GetDesktopResolution(int* pHorizontal, int* pVertical)
	{
		RECT desktopRectangle;
		const HWND desktopHandle = GetDesktopWindow();

		if (desktopHandle && GetWindowRect(desktopHandle, &desktopRectangle))
		{
			*pHorizontal = desktopRectangle.right;
			*pVertical = desktopRectangle.bottom;
			return true;
		}
		return false;
	}

	typedef struct vidmode_s
	{
		const char* description;
		int width, height;
		float pixelAspect;
	} vidmode_t;

	vidmode_t r_vidModes[] =
	{
		//stock CoD1 modes
		{ "Mode  0:   320x240   (4:3)",     320,     240,    1 },
		{ "Mode  1:   400x300   (4:3)",     400,     300,    1 },
		{ "Mode  2:   512x384   (4:3)",     512,     384,    1 },
		{ "Mode  3:   640x480   (4:3)",     640,     480,    1 },
		{ "Mode  4:   800x600   (4:3)",     800,     600,    1 },
		{ "Mode  5:   960x720   (4:3)",     960,     720,    1 },
		{ "Mode  6:  1024x768   (4:3)",    1024,     768,    1 },
		{ "Mode  7:  1152x864   (4:3)",    1152,     864,    1 },
		{ "Mode  8: 1280x1024   (5:4)",    1280,    1024,    1 },
		{ "Mode  9: 1600x1200   (4:3)",    1600,    1200,    1 },
		{ "Mode 10: 2048x1536   (4:3)",    2048,    1536,    1 },
		{ "Mode 11:   856x480  (16:9)",     856,     480,    1 },
		{ "Mode 12: 1920x1200 (16:10)",	   1920,    1200,    1 },
		// additional modes taken from iortcw 
		{ "Mode 13:   640x360  (16:9)",     640,     360,    1 },
		{ "Mode 14:   640x400 (16:10)",     640,     400,    1 },
		{ "Mode 15:   800x450  (16:9)",     800,     450,    1 },
		{ "Mode 16:   800x500 (16:10)",     800,     500,    1 },
		{ "Mode 17:  1024x640 (16:10)",    1024,     640,    1 },
		{ "Mode 18:  1024x576  (16:9)",    1024,     576,    1 },
		{ "Mode 19:  1280x720  (16:9)",    1280,     720,    1 },
		{ "Mode 20:  1280x768 (16:10)",    1280,     768,    1 },
		{ "Mode 21:  1280x800 (16:10)",    1280,     800,    1 },
		{ "Mode 22:  1280x960   (4:3)",    1280,     960,    1 },
		{ "Mode 23:  1440x900 (16:10)",    1440,     900,    1 },
		{ "Mode 24:  1600x900  (16:9)",    1600,     900,    1 },
		{ "Mode 25: 1600x1000 (16:10)",    1600,    1000,    1 },
		{ "Mode 26: 1680x1050 (16:10)",    1680,    1050,    1 },
		{ "Mode 27: 1920x1080  (16:9)",    1920,    1080,    1 },
		{ "Mode 28: 1920x1440   (4:3)",    1920,    1440,    1 },
		{ "Mode 29: 2560x1600 (16:10)",    2560,    1600,    1 }
	};

	static int s_numVidModes = (sizeof(r_vidModes) / sizeof(r_vidModes[0]));

	boolean R_GetModeInfo(int mode, int* height, float* windowAspect, int* width)
	{
		vidmode_t* vm;
		float			pixelAspect;
		mode = r_mode->integer;
		width = game::vidWidth;
		height = game::vidHeight;

		//if mode doesn't exist use an existing mode, having it set to false would cause a openGL error.
		if (mode < -1 || mode >= s_numVidModes + 1)
			mode = 6;

		if (mode == -1)
		{
			*width = r_customwidth->integer;
			*height = r_customheight->integer;
			pixelAspect = r_customaspect->value;
			return true;
		}
		if (mode == 30)
		{
			GetDesktopResolution(width, height);

			if (*height > *width)  // incase someone has their monitor in portrait mode
				*height = (int)(*width / (4.0 / 3.0));

			*windowAspect = (float)*width / *height;
			return true;
		}
		else
		{
			vm = &r_vidModes[mode];
			*width = vm->width;
			*height = vm->height;
			pixelAspect = vm->pixelAspect;
		}

		*windowAspect = (float)vm->width / (vm->height * vm->pixelAspect);
		return true;
	}

	void R_ModeList_f()
	{
		int i;
		game::Com_Printf("\n");
		for (i = 0; i < s_numVidModes; i++)
		{
			game::Com_Printf("%s\n", r_vidModes[i].description);
		}

		game::Com_Printf("Mode 30: Automatic (Native)\n");
		game::Com_Printf("Mode -1: Custom\n");
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			r_mode = game::Cvar_Get("r_mode", "3", game::CVAR_ARCHIVE | game::CVAR_LATCH);
			r_customwidth = game::Cvar_Get("r_customwidth", "1600", game::CVAR_ARCHIVE | game::CVAR_LATCH);
			r_customheight = game::Cvar_Get("r_customheight", "1024", game::CVAR_ARCHIVE | game::CVAR_LATCH);
			r_customaspect = game::Cvar_Get("r_customaspect", "1", game::CVAR_ARCHIVE | game::CVAR_LATCH);
			utils::hook::jump(0x4b0ec0, R_GetModeInfo);
			utils::hook::jump(0x4b0f30, R_ModeList_f);
		}
	};
}

REGISTER_COMPONENT(vidmode::component)