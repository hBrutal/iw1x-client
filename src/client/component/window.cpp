#include <std_include.hpp>
#if 1
#include <utils/hook.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "imgui.hpp"

#include <utils/string.hpp>






//#include <hidusage.h>



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace window
{





#if 0
	int mouse_raw_x = 0;
	int mouse_raw_y = 0;
	game::cvar_t* cl_rawInput;
	void in_clamp_mouse_move(tagPOINT& current_position)
	{
		tagRECT rc;
		GetWindowRect(*game::hWnd, &rc);

		auto is_clamped = false;
		if (current_position.x >= rc.left)
		{
			if (current_position.x >= rc.right)
			{
				current_position.x = rc.right - 1;
				is_clamped = true;
			}
		}
		else
		{
			current_position.x = rc.left;
			is_clamped = true;
		}
		if (current_position.y >= rc.top)
		{
			if (current_position.y >= rc.bottom)
			{
				current_position.y = rc.bottom - 1;
				is_clamped = true;
			}
		}
		else
		{
			current_position.y = rc.top;
			is_clamped = true;
		}

		if (is_clamped)
		{
			SetCursorPos(current_position.x, current_position.y);
		}
	}
	void CL_MouseEvent(int dx, int dy)
	{
		_asm
		{
			mov eax, 0x0040b0a0
			call eax
			add esp, 0x8
		}
	}
	void in_raw_mouse_move()
	{
		static const auto* r_fullscreen = game::Cvar_FindVar("r_fullscreen");

		if (GetForegroundWindow() == *game::hWnd)
		{
			static tagPOINT current_position;
			GetCursorPos(&current_position);

			// is fullscreen
			if (r_fullscreen->integer == 1)
			{
				in_clamp_mouse_move(current_position);
			}

			//static auto old_x = 0, old_y = 0;

			//const auto need_reset = game::s_wmv->oldPos.x == -100000;

			//const auto delta_x = need_reset ? 0 : mouse_raw_x - old_x;
			//const auto delta_y = need_reset ? 0 : mouse_raw_y - old_y;

			//old_x = mouse_raw_x;
			//old_y = mouse_raw_y;

			//game::s_wmv->oldPos = current_position;
			ScreenToClient(*game::hWnd, &current_position);

			//current_position.x = current_position.x * game::vidConfigOut->displayWidth / game::vidConfigOut->monitorWidth;
			//current_position.y = current_position.y * game::vidConfigOut->displayHeight / game::vidConfigOut->monitorHeight;


			CL_MouseEvent(current_position.x, current_position.y);



			/*game::g_wv->recenterMouse = game::CL_MouseEvent(current_position.x, current_position.y, delta_x, delta_y);

			if (game::g_wv->recenterMouse && (delta_x || delta_y || need_reset))
			{
				game::IN_RecenterMouse();
			}*/
		}
	}
	void in_mouse_move()
	{
		if (cl_rawInput->integer)
		{
			in_raw_mouse_move();
		}
		else
		{
			game::IN_MouseMove();
		}
	}
	void in_raw_mouse_init(HWND hwnd)
	{
		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		rid[0].dwFlags = RIDEV_INPUTSINK;
		rid[0].hwndTarget = hwnd;

		RegisterRawInputDevices(rid, ARRAYSIZE(rid), sizeof(rid[0]));
	}
#endif







	HHOOK hHook;

	utils::hook::detour Com_Init_hook;

	LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (GetForegroundWindow() == *game::hWnd)
		{
			if (nCode == HC_ACTION)
			{
				auto pKey = (KBDLLHOOKSTRUCT*)lParam;

				auto altDown = (pKey->flags & LLKHF_ALTDOWN);
				auto escDown_sys = (pKey->vkCode == VK_ESCAPE && wParam == WM_SYSKEYDOWN);
				auto escUp_sys = (pKey->vkCode == VK_ESCAPE && wParam == WM_SYSKEYUP);
				
				if (altDown)
				{
					if (escDown_sys)
					{
						if (!imgui::waitForMenuKeyRelease)
						{
							imgui::toggle_menu_flag();
							imgui::waitForMenuKeyRelease = true;
						}
						return 1; // Prevent Windows Alt+Esc behavior
					}
					else if (escUp_sys)
					{
						imgui::waitForMenuKeyRelease = false;
					}
				}
				else if (imgui::waitForMenuKeyRelease)
				{
					// Released Alt before releasing Esc
					imgui::waitForMenuKeyRelease = false;
				}
			}
		}

		return CallNextHookEx(hHook, nCode, wParam, lParam);
	}

	LRESULT CALLBACK MainWndProc_stub(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (/*imgui::displayed && */ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		
		switch (uMsg)
		{





		/*case WM_INPUT:
		{

			std::uint32_t size = sizeof(RAWINPUT);
			static BYTE lpb[sizeof(RAWINPUT)];
			GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT, lpb, &size,
				sizeof(RAWINPUTHEADER)
			);
			auto* raw = reinterpret_cast<RAWINPUT*>(lpb);
			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				// Is there's really absolute mouse on earth?
				if (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
				{
					mouse_raw_x = raw->data.mouse.lLastX;
					mouse_raw_y = raw->data.mouse.lLastY;
				}
				else
				{
					mouse_raw_x += raw->data.mouse.lLastX;
					mouse_raw_y += raw->data.mouse.lLastY;
				}
			}
			return TRUE;
		}*/



		case WM_CREATE:
			SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_MINIMIZEBOX); // Add minimize button and Win+M support


			//in_raw_mouse_init(hWnd);


			break;
		case WM_CHAR:
			if (wParam == VK_ESCAPE)
			{
				if (imgui::displayed)
				{
					// Allow closing imgui by pressing only Esc
					imgui::toggle_menu_flag();
				}
			}
			break;
		case WM_KEYDOWN:
			if (imgui::displayed)
				return 0; // Prevent moving
			break;
		case WM_MOUSEWHEEL:
			if (imgui::displayed)
				return 0; // Prevent changing weapon
			break;
		case WM_MENUCHAR:
			if (imgui::displayed)
				return MNC_CLOSE << 16; // Prevent Alt+Enter beep sound
			break;
		}
		
		if (imgui::displayed)
		{
			/*
			When opening imgui, and not using the below code, it would fail getting focus (except if Alt+Esc was pressed in a particular way)
			See https://github.com/kartjom/CoDPlusPlus/blob/359539f889958b2cbd58884cbc5bb0e3e5a3c294/CoDPlusPlus/src/Utils/WinApiHelper.cpp#L210
			*/
			if (wParam == SC_KEYMENU && (lParam >> 16) <= 0)
				return 0;
		}
		
		return game::MainWndProc(hWnd, uMsg, wParam, lParam);
	}
	
	void Com_Init_stub(char* commandLine)
	{
		hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
		if (!hHook)
			throw std::runtime_error(utils::string::va("SetWindowsHookEx for LowLevelKeyboardProc failed"));
		
		Com_Init_hook.invoke(commandLine);
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			utils::hook::set(0x4639b9 + 1, MainWndProc_stub);
			utils::hook::set(0x5083b1, 0x00); // Alt+Tab support, see https://github.com/xtnded/codextended-client/pull/1
			
			Com_Init_hook.create(0x004375c0, Com_Init_stub);




			
			/*utils::hook::call(0x00461ae7, &in_mouse_move);
			cl_rawInput = game::Cvar_Get("cl_rawInput", "0", CVAR_ARCHIVE);*/


			//utils::hook::nop(0x00466d0f, 5); // WIN_DisableAltTab
		}
	};
}

REGISTER_COMPONENT(window::component)
#endif