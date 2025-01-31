#include <std_include.hpp>
#if 0
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "imgui.hpp"

#include <utils/hook.hpp>

namespace game_window
{
	LRESULT CALLBACK wnd_proc_stub(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{


		/*if (uMsg != WM_KEYDOWN || wParam != VK_HOME) // Not to send toggle key to imgui
		{
			if (imgui::displayed && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			{
				return true;
			}
		}*/

		if (imgui::displayed && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		{
			return true;
		}

		switch (uMsg)
		{
		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_HOME:
				if (!imgui::waitForMenuKeyRelease)
				{
					if (!imgui::displayed)
					{
						imgui::doDisplay = true;

						game::IN_DeactivateMouse();
						
						*game::mouseActive = game::qfalse;
						*game::mouseInitialized = game::qfalse;

						
					}
					else
					{
						//ImGui::SaveIniSettingsToDisk(ImGui::GetIO().IniFilename); // Not to wait for IniSavingRate
						imgui::doDisplay = false;
						*game::mouseInitialized = game::qtrue;
						game::IN_ActivateMouse();
					}
				}
				imgui::waitForMenuKeyRelease = true;
				break;
			}
			break;
		case WM_KEYUP:
			switch (wParam)
			{
			case VK_HOME:
				imgui::waitForMenuKeyRelease = false;
				break;
			default:
				break;
			}
			break;
		}




		return game::MainWndProc(hWnd, uMsg, wParam, lParam);
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
			
			utils::hook::set(0x4639b9 + 1, wnd_proc_stub);
		}
	};
}

REGISTER_COMPONENT(game_window::component)
#endif