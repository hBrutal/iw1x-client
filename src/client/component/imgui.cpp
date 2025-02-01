#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "imgui.hpp"

namespace imgui
{
	bool initialized = false;
	bool displayed = false;
	bool waitForMenuKeyRelease = false;
	HGLRC imguiWglContext;
	HWND hWnd_during_init;
	
	void new_frame()
	{
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	
	void draw_menu()
	{
		ImGui::SetNextWindowSize(ImVec2(100, 100));
		ImGui::SetNextWindowPos(ImVec2(50, 150), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowFocus();
		ImGui::Begin(MOD_NAME, NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::End();
	}

	void end_frame()
	{
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	}

	void gui_on_frame()
	{
		new_frame();
		draw_menu();
		end_frame();
	}

	void init(HDC hdc)
	{
		hWnd_during_init = *game::hWnd;
		imguiWglContext = wglCreateContext(hdc);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_InitForOpenGL(*game::hWnd);
		ImGui_ImplOpenGL2_Init();

		initialized = true;
	}

	void shutdown()
	{
		if (initialized)
		{
			ImGui_ImplOpenGL2_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			initialized = false;
		}
	}
	
	BOOL WINAPI SwapBuffers_stub(HDC hdc)
	{
		if (!initialized)
		{
			init(hdc);
		}
		else
		{
			if (WindowFromDC(hdc) != hWnd_during_init) // Caused by Alt+Enter / vid_restart
			{
				ImGui_ImplOpenGL2_Shutdown();
				ImGui_ImplWin32_Shutdown();
				ImGui::DestroyContext();
				init(hdc);
			}
		}

		if (displayed)
		{
			auto originalWglContext = wglGetCurrentContext();

			wglMakeCurrent(hdc, imguiWglContext);
			gui_on_frame();
			wglMakeCurrent(hdc, originalWglContext);
		}
		
		return SwapBuffers(hdc);
	}
	
	class component final : public component_interface
	{
	public:
		void* load_import(const std::string&, const std::string& function) override
		{
			if (function == "SwapBuffers" && (!game::environment::is_dedi() && !game::environment::is_sp()))
			{
				return SwapBuffers_stub;
			}
			return nullptr;
		}
		
		void pre_destroy() override
		{
			if (game::environment::is_dedi() || game::environment::is_sp())
				return;
			shutdown();
		}
	};
}

REGISTER_COMPONENT(imgui::component)