#include <std_include.hpp>
#if 0
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "component/scheduler.hpp"
#include "imgui.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/concurrency.hpp>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace imgui
{
	BOOL(WINAPI* originalSwapBuffers)(HDC);

	bool initialized = false;
	bool doDisplay = false;
	bool displayed = false;
	bool waitForMenuKeyRelease = false;
	

	
	


	
	/*struct frame_callback
	{
		std::function<void()> callback;
		bool always;
	};

	struct event
	{
		HWND hWnd;
		UINT msg;
		WPARAM wParam;
		LPARAM lParam;
	};

	utils::concurrency::container<std::vector<frame_callback>> on_frame_callbacks;
	utils::concurrency::container<std::vector<event>> event_queue;*/
		
		
		
	HGLRC imguiWglContext;
		
	/*void run_event_queue()
	{
		event_queue.access([](std::vector<event>& queue)
			{
				for (const auto& event : queue)
				{
					ImGui_ImplWin32_WndProcHandler(event.hWnd, event.msg, event.wParam, event.lParam);
				}
				queue.clear();
			});
	}*/

	void new_gui_frame()
	{
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplWin32_NewFrame();

		//run_event_queue();

		ImGui::NewFrame();
	}

	void end_gui_frame()
	{
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	}

	/*void run_frame_callbacks()
	{
		on_frame_callbacks.access([](std::vector<frame_callback>& callbacks)
			{
				for (const auto& callback : callbacks)
				{
					if (callback.always || displayed)
					{
						callback.callback();
					}
				}
			});
	}*/
		
	void draw_menu()
	{
		ImGui::BeginChild("Empty Box", ImVec2(200, 100), true);
		ImGui::EndChild();
	}

	void gui_on_frame(HDC hdc)
	{
		new_gui_frame();

		//run_frame_callbacks();
		draw_menu();

		end_gui_frame();
	}


	void shutdown_gui()
	{
		if (initialized)
		{
			ImGui_ImplOpenGL2_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}
		initialized = false;
	}
		
	void init(HDC hdc)
	{
		imguiWglContext = wglCreateContext(hdc);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_InitForOpenGL(*game::hWnd);
		ImGui_ImplOpenGL2_Init();

		initialized = true;
	}
		
	BOOL APIENTRY SwapBuffers_stub(HDC hdc)
	{
		if (!initialized)
		{
			init(hdc);
		}
			
		if (!doDisplay)
		{
			displayed = false;
		}
		else
		{
			HGLRC originalWglContext = wglGetCurrentContext();

			wglMakeCurrent(hdc, imguiWglContext);
			gui_on_frame(hdc);
			wglMakeCurrent(hdc, originalWglContext);

			displayed = true;
		}

		return originalSwapBuffers(hdc);
	}






	/*LRESULT CALLBACK wnd_proc_stub(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (wParam != VK_ESCAPE && toggled)
		{
			event_queue.access([hWnd, msg, wParam, lParam](std::vector<event>& queue)
				{
					queue.emplace_back(hWnd, msg, wParam, lParam);
				});
		}

		return game::MainWndProc(hWnd, msg, wParam, lParam);
	}*/


	


	/*void on_frame(const std::function<void()>& callback, bool always)
	{
		on_frame_callbacks.access([always, callback](std::vector<frame_callback>& callbacks)
			{
				callbacks.emplace_back(callback, always);
			});
	}*/

	/*void register_callback(const std::function<void()>& callback, bool always)
	{
		on_frame([=]
			{
				callback();
			}, always);
	}*/



	class component final : public component_interface
	{
	public:
		void* load_import(const std::string& library, const std::string& function) override
		{
			if (function == "SwapBuffers" && (!game::environment::is_dedi() && !game::environment::is_sp()))
			{
				const utils::nt::library lib(library);
				originalSwapBuffers = (BOOL(APIENTRY*)(HDC))GetProcAddress(lib, function.c_str());
				return SwapBuffers_stub;
			}
			return nullptr;
		}

		void post_unpack() override
		{
			if (game::environment::is_dedi() || game::environment::is_sp())
				return;

			//draw_menu();
			/*on_frame([]
			{
					draw_menu();
			});*/
		}

		void pre_destroy() override
		{
			if (game::environment::is_dedi() || game::environment::is_sp())
				return;

			shutdown_gui();
		}
	};
}

REGISTER_COMPONENT(imgui::component)
#endif