#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include <utils/string.hpp>
#include "imgui.hpp"

#include "movement.hpp"

namespace imgui
{
	bool initialized = false;
	bool displayed = false;
	bool waitForMenuKeyRelease = false;
	HGLRC imguiWglContext;
	HWND hWnd_during_init;

	bool sensitivity_adsScaleEnable = false;
	float sensitivity_adsScale = 0.0f;
	bool sensitivity_adsScaleSniperEnable = false;
	float sensitivity_adsScaleSniper = 0.0f;
	
	void new_frame()
	{
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	
	void menu_loads_settings()
	{
		sensitivity_adsScaleEnable = movement::sensitivity_adsScaleEnable->integer;
		sensitivity_adsScale = movement::sensitivity_adsScale->value;
		sensitivity_adsScaleSniperEnable = movement::sensitivity_adsScaleSniperEnable->value;
		sensitivity_adsScaleSniper = movement::sensitivity_adsScaleSniper->value;
	}
	
	void draw_menu()
	{
		menu_loads_settings();

		ImGui::SetNextWindowSize(ImVec2(300, 0));
		ImGui::SetNextWindowPos(ImVec2(50, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowFocus();
		ImGui::Begin(MOD_NAME, NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		// Sensitivity multiplier
		ImGui::SeparatorText("Aim Down Sight");
		ImGui::Checkbox("Sensitivity multiplier", &sensitivity_adsScaleEnable);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (!sensitivity_adsScaleEnable)
			ImGui::BeginDisabled();
		ImGui::SliderFloat("##slider_sensitivity_adsScale", &sensitivity_adsScale, 0.15f, 1.30f, "%.2f");
		if (!sensitivity_adsScaleEnable)
			ImGui::EndDisabled();

		ImGui::Spacing();

		// Sensitivity sniper multiplier
		ImGui::Checkbox("Sensitivity sniper multiplier", &sensitivity_adsScaleSniperEnable);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (!sensitivity_adsScaleSniperEnable)
			ImGui::BeginDisabled();
		ImGui::SliderFloat("##slider_sensitivity_adsScaleSniper", &sensitivity_adsScaleSniper, 0.15f, 1.30f, "%.2f");
		if (!sensitivity_adsScaleSniperEnable)
			ImGui::EndDisabled();

		ImGui::End();
		menu_updates_settings();
	}

	void menu_updates_settings()
	{
		game::Cvar_Set(movement::sensitivity_adsScaleEnable->name, sensitivity_adsScaleEnable ? "1" : "0");
		game::Cvar_Set(movement::sensitivity_adsScale->name, utils::string::va("%.2f", sensitivity_adsScale));
		game::Cvar_Set(movement::sensitivity_adsScaleSniperEnable->name, sensitivity_adsScaleSniperEnable ? "1" : "0");
		game::Cvar_Set(movement::sensitivity_adsScaleSniper->name, utils::string::va("%.2f", sensitivity_adsScaleSniper));
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
			shutdown();
		}
	};
}

REGISTER_COMPONENT(imgui::component)