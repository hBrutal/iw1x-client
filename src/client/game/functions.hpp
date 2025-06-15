namespace game
{
	int Cmd_Argc();
	char* Cmd_Argv(int arg);
	weaponInfo_t* BG_GetInfoForWeapon(int weaponNum);
	const char* Info_ValueForKey(const char* buffer, const char* key);
	void CL_MouseEvent(int _dx, int _dy);
	void PM_ClipVelocity(vec3_t _in, vec3_t normal, vec3_t _out, float overbounce);

	WEAK symbol<LRESULT CALLBACK(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)> MainWndProc{ 0x466BE0 };
	WEAK symbol<int(const char *var_name)>Cvar_VariableIntegerValue{ 0x00439280 };
	WEAK symbol<cvar_t* (const char* name)> Cvar_FindVar{ 0x00439280 };
	WEAK symbol<cvar_t* (const char* name, const char* value, int flags)> Cvar_Get{ 0x00439350 };
	WEAK symbol<cvar_t* (const char* name, const char* value)> Cvar_Set{ 0x00439650 };
	WEAK symbol<cvar_t* (byte* param_1)> va{ 0x0044ACE0 };
	WEAK symbol<void(int code, const char* fmt, ...)> Com_Error{ 0x00435ad0 };
	WEAK symbol<void(const char* msg, ...)> Com_Printf{ 0x004357b0 };
	WEAK symbol<void(const char* cmd_name, xcommand_t function)> Cmd_AddCommand{ 0x00428840 };
	WEAK symbol<void(cbufExec_t exec_when, const char* text)> Cbuf_ExecuteText{ 0x00428290 };
	WEAK symbol<void()> IN_DeactivateMouse{ 0x4616b0 };
	WEAK symbol<void()> IN_ActivateMouse{ 0x461730 };
	WEAK symbol<const char*(netadr_t a)> NET_AdrToString{ 0x00449150 };
	WEAK symbol<qboolean(netadr_t a, netadr_t b)> NET_CompareAdr{ 0x00449230 };
	WEAK symbol<void(float x, float y, int fontID, float scale, float* color, const char* text, float spaceBetweenChars, int maxChars, int arg9)> SCR_DrawString{ 0x004df570 };
	WEAK symbol<shader_t*(char* name, int param_2, int param_3, int param_4)> R_FindShader{ 0x00449150 };
	WEAK symbol<shader_t* (char* name, qhandle_t hShader)> RE_RegisterShaderNoMip{ 0x004fcae0 };
	//CGAME
	WEAK symbol<void(float x, float y, float width, float height, qhandle_t hShader)> CG_DrawPic{ 0x300192d0, BASE_CGAME_MP };
	WEAK symbol<void(int, char const*)> CG_PlayClientSoundAliasByName{ 0x30021bf0, BASE_CGAME_MP };
	WEAK symbol<void(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader)> trap_R_stretchpic{ 0x300310f0, BASE_CGAME_MP };
	WEAK symbol<void(float x, float y, float w, float h, float Angle,float position)> CG_DrawRotatedPic{ 0x30019330, BASE_CGAME_MP };
	WEAK symbol<void()> CG_UpdateCompassOrientation{ 0x30019bd0, BASE_CGAME_MP };

	//UI_MP
	WEAK symbol<void(int _dx, int _dy)> Display_MouseMove{ 0x40018b40, BASE_UI_MP };
	WEAK symbol<int()> Menu_Count{ 0x400189a0, BASE_UI_MP };
}
