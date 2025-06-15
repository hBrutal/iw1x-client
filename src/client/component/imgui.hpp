#pragma once
namespace imgui
{
	extern bool displayed;
	extern bool waitForMenuKeyRelease;

	void menu_updates_settings();
	void toggle_menu(bool closedUsingButton);
	void new_frame();
	void draw_menu();
	void draw_menu_tab_Security();
	void draw_menu_tab_UI();
	void draw_menu_tab_View();
	void draw_menu_tab_Movement();
	void draw_menu_tab_Social();
	void end_frame();
}