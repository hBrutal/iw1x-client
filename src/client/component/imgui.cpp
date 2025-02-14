#include <std_include.hpp>
#if 1
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include <utils/string.hpp>
#include "imgui.hpp"

#include "security.hpp"
#include "monitoring.hpp"
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
	bool cg_drawDisconnect = true;
	bool cg_drawWeaponSelect = true;
	bool cg_drawFPS = false;
	int cg_chatHeight = 8;
	int con_boldgamemessagetime = 8;
	bool cg_lagometer = false;
	bool cl_allowDownload = false;

	void toggle_menu_flag()
	{
		if (!displayed)
		{
			displayed = true;
			game::IN_DeactivateMouse();
			*reinterpret_cast<int*>(0x8e2520) = 0; // mouseActive
			*reinterpret_cast<int*>(0x8e2524) = 0; // mouseInitialized
		}
		else
		{
			displayed = false;
			*reinterpret_cast<int*>(0x8e2524) = 1; // mouseInitialized
			game::IN_ActivateMouse();
		}
	}

	void init(HDC hdc)
	{
		hWnd_during_init = *game::hWnd;
		imguiWglContext = wglCreateContext(hdc);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 16.0f);
		ImGui_ImplWin32_InitForOpenGL(*game::hWnd);
		ImGui_ImplOpenGL2_Init();

		initialized = true;
	}

	void gui_on_frame()
	{
		new_frame();
		draw_menu();
		end_frame();
	}
	
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
		cg_drawDisconnect = monitoring::cg_drawDisconnect->integer;
		cg_drawWeaponSelect = monitoring::cg_drawWeaponSelect->integer;
		cg_drawFPS = monitoring::cg_drawFPS->integer;
		cg_chatHeight = monitoring::cg_chatHeight->integer;
		con_boldgamemessagetime = monitoring::con_boldgamemessagetime->integer;
		cg_lagometer = monitoring::cg_lagometer->integer;
		cl_allowDownload = security::cl_allowDownload->integer;
	}

	void draw_menu()
	{
		menu_loads_settings();

		ImGui::SetNextWindowSize(ImVec2(300, 0));
		ImGui::SetNextWindowPos(ImVec2(50, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowFocus();
		ImGui::Begin(MOD_NAME, NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		//// Security
		ImGui::SeparatorText("Security");
		ImGui::Checkbox("Downloading", &cl_allowDownload);
		////

		// Spacing
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		//// Interface
		ImGui::SeparatorText("Interface");
		ImGui::Checkbox("FPS", &cg_drawFPS);
		ImGui::Checkbox("Lagometer", &cg_lagometer);
		ImGui::Checkbox("\"Connection Interrupted\"", &cg_drawDisconnect);
		ImGui::Checkbox("Weapon selection", &cg_drawWeaponSelect);

		ImGui::Spacing();

		ImGui::Text("Chat lines");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::SliderInt("##slider_cg_chatHeight", &cg_chatHeight, 0, 8, "%i", ImGuiSliderFlags_NoInput);

		ImGui::Spacing();

		ImGui::Text("Middle messages seconds");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::SliderInt("##slider_con_boldgamemessagetime", &con_boldgamemessagetime, 0, 8, "%i", ImGuiSliderFlags_NoInput);
		////

		// Spacing
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		//// ADS
		ImGui::SeparatorText("Aim Down Sight");
		// Sensitivity multiplier
		ImGui::Checkbox("Sensitivity multiplier", &sensitivity_adsScaleEnable);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (!sensitivity_adsScaleEnable)
			ImGui::BeginDisabled();
		ImGui::SliderFloat("##slider_sensitivity_adsScale", &sensitivity_adsScale, 0.15f, 1.30f, "%.2f", ImGuiSliderFlags_NoInput);
		if (!sensitivity_adsScaleEnable)
			ImGui::EndDisabled();

		ImGui::Spacing();

		// Sensitivity sniper multiplier
		ImGui::Checkbox("Sensitivity sniper multiplier", &sensitivity_adsScaleSniperEnable);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (!sensitivity_adsScaleSniperEnable)
			ImGui::BeginDisabled();
		ImGui::SliderFloat("##slider_sensitivity_adsScaleSniper", &sensitivity_adsScaleSniper, 0.15f, 1.30f, "%.2f", ImGuiSliderFlags_NoInput);
		if (!sensitivity_adsScaleSniperEnable)
			ImGui::EndDisabled();
		////

		ImGui::End();
		menu_updates_settings();
	}

	void menu_updates_settings()
	{
		game::Cvar_Set(movement::sensitivity_adsScaleEnable->name, sensitivity_adsScaleEnable ? "1" : "0");
		game::Cvar_Set(movement::sensitivity_adsScale->name, utils::string::va("%.2f", sensitivity_adsScale));
		game::Cvar_Set(movement::sensitivity_adsScaleSniperEnable->name, sensitivity_adsScaleSniperEnable ? "1" : "0");
		game::Cvar_Set(movement::sensitivity_adsScaleSniper->name, utils::string::va("%.2f", sensitivity_adsScaleSniper));
		game::Cvar_Set(monitoring::cg_drawDisconnect->name, cg_drawDisconnect ? "1" : "0");
		game::Cvar_Set(monitoring::cg_drawWeaponSelect->name, cg_drawWeaponSelect ? "1" : "0");
		game::Cvar_Set(monitoring::cg_drawFPS->name, cg_drawFPS ? "1" : "0");
		game::Cvar_Set(monitoring::cg_chatHeight->name, utils::string::va("%i", cg_chatHeight));
		game::Cvar_Set(monitoring::con_boldgamemessagetime->name, utils::string::va("%i", con_boldgamemessagetime));
		game::Cvar_Set(monitoring::cg_lagometer->name, cg_lagometer ? "1" : "0");
		game::Cvar_Set(security::cl_allowDownload->name, cl_allowDownload ? "1" : "0");
	}

	void end_frame()
	{
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
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
			if (function == "SwapBuffers")
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
#endif