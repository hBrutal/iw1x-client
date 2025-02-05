#include <std_include.hpp>
#if 1
#include <utils/hook.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "imgui.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace game_window
{
	LRESULT CALLBACK MainWndProc_stub(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//if (uMsg != WM_KEYDOWN || wParam != VK_HOME) // Not to send toggle key to imgui
		//{
			if (imgui::displayed && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
				return true;
		//}
		
		switch (uMsg)
		{
		case WM_CREATE:
			SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_MINIMIZEBOX); // Adds minimize button and Win+M support
			break;
		case WM_KEYDOWN:
			if (wParam == VK_HOME)
			{
				if (!imgui::waitForMenuKeyRelease)
				{
					if (!imgui::displayed)
					{
						imgui::displayed = true;
						game::IN_DeactivateMouse();
						*reinterpret_cast<int*>(0x8e2520) = 0; // mouseActive
						*reinterpret_cast<int*>(0x8e2524) = 0; // mouseInitialized
					}
					else
					{
						imgui::displayed = false;
						*reinterpret_cast<int*>(0x8e2524) = 1; // mouseInitialized
						game::IN_ActivateMouse();
					}
				}
				imgui::waitForMenuKeyRelease = true;
				return 0;
			}
			else
			{
				if (imgui::displayed)
					return 0; // Prevents player from moving
			}
			break;
		case WM_KEYUP:
			if (wParam == VK_HOME)
			{
				if (imgui::waitForMenuKeyRelease)
				{
					imgui::waitForMenuKeyRelease = false;
					return 0;
				}
			}
			break;
		case WM_MOUSEWHEEL:
			if (imgui::displayed)
				return 0; // Prevents player from changing weapon
			break;
		case WM_MENUCHAR:
			if (imgui::displayed)
				return MNC_CLOSE << 16; // Prevents Alt+Enter beep sound
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
				return;

			utils::hook::set(0x4639b9 + 1, MainWndProc_stub);
			utils::hook::set(0x5083b1, 0x00); // Alt+Tab support, see https://github.com/xtnded/codextended-client/pull/1
		}
	};
}

REGISTER_COMPONENT(game_window::component)
#endif