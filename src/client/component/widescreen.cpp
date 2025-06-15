#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "widescreen.hpp"
#include "scheduler.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace widescreen
{
	game::cvar_t* cg_fixedAspect;

	utils::hook::detour R_stretchpic_hook;

	float g_screenW = 0.0f;
	float g_screenH = 0.0f;
	float g_screenAspect = 0.0f;
	float g_baseAspect = 1.3333334f;
	float g_baseWidth = 0.0f;
	float g_offset = 0.0f;

	void UpdateAspectGlobals()
	{
		g_screenW = static_cast<float>(*game::vidWidth);
		g_screenH = static_cast<float>(*game::vidHeight);
		g_screenAspect = g_screenW / g_screenH;
		g_baseWidth = g_screenH * g_baseAspect;
		g_offset = 0.5f * (g_screenW - g_baseWidth);
	}

	static void R_stretchpic_Stub(float x, float y, float w, float h, float s1, float t1, float s2, float t2, game::qhandle_t hShader)
	{
		float X1 = x;
		float W1 = w;

		if ((cg_fixedAspect->integer && g_screenAspect > g_baseAspect) && (hShader != 36 && hShader != 39))	//36 is unknown map, 39 is levelshot
		{
			float scaleX = g_baseWidth / g_screenW;
			float baseX = (x / g_screenW) * g_baseWidth;
			W1 = w * scaleX;
			X1 = baseX + g_offset;
		}

		R_stretchpic_hook.invoke(X1, y, W1, h, s1, t1, s2, t2, hShader);
	} 

	static void R_stretchpic_bypass(float x, float y, float w, float h, float s1, float t1, float s2, float t2, game::qhandle_t hShader)
	{
		R_stretchpic_hook.invoke(x, y, w, h, s1, t1, s2, t2, hShader);
	}

	static void SCR_AdjustFrom640(float* x, float* y, float* w, float* h)
	{
		float scaleX, offset = 0.0f;
		float scaleY = g_screenH / 480.0f;

		if (cg_fixedAspect->integer && g_screenAspect > g_baseAspect)
		{
			float baseWidth = (g_screenH * 4.0f) / 3.0f;
			scaleX = baseWidth / 640.0f;

			if (g_screenW * 480 > g_screenH * 640)
			{
				offset = 0.5f * (g_screenW - baseWidth);
			}
		}
		else
		{
			scaleX = *game::vidWidth / 640.0f;
		}

		if (x) *x *= scaleX;
		if (y) *y *= scaleY;
		if (w) *w *= scaleX;
		if (h) *h *= scaleY;
		if (x) *x += offset;
	}

	static void SCR_DrawPic(float x, float y, float w, float h, game::qhandle_t hShader)
	{
		float X1 = x;
		float W1 = w;

		if (cg_fixedAspect->integer && g_screenAspect > g_baseAspect)
		{
			X1 = x - (g_offset * 0.5f);
			W1 = w + g_offset;
		}

		SCR_AdjustFrom640(&X1, &y, &W1, &h);
		R_stretchpic_hook.invoke(X1, y, W1, h, 0, 0, 1, 1, hShader);
	}
	
	static void CG_DrawDamage_Stub(float x, float y, float w, float h, const float* color)
	{
		float X1 = x;
		float W1 = w;
		if (cg_fixedAspect->integer && g_screenAspect > g_baseAspect)
		{
			X1 = x - g_offset;
			W1 = w + g_offset * 2;
		}

		utils::hook::invoke<void>(ABSOLUTE_CGAME_MP(0x30018d10), X1, y, W1, h, color); // Draw normally
	}

	static void CG_DrawCrosshair_Stub(float x, float y, float w, float h, float s1, float t1, float s2, float t2, game::qhandle_t hShader)
	{
		float W1 = w;
		float X1 = x;
		if (cg_fixedAspect->integer && g_screenAspect > g_baseAspect)
		{
			float scale = g_baseWidth / g_screenW;
			W1 = w / scale;
			float wdiff = W1 - w;
			X1 = x - ( wdiff * 0.5f );
		}

		utils::hook::invoke<void>(ABSOLUTE_CGAME_MP(0x300310f0), X1, y, W1, h, s1, t1, s2, t2, hShader);
	}

	static void CG_DrawRotatedPic(float X, float Y, float width, float height, float angle, game::qhandle_t hShader)
	{
		float X1 = X;
		float W1 = width;
		float XScale = *game::cg_refdef_XScale;
		float YScale = *game::cg_refdef_YScale;

		if (cg_fixedAspect->integer && g_screenAspect > g_baseAspect)
		{
			XScale = 2.25f;
			X1 = X + 107;	
		}

		float screenX = XScale * X1;
		float screenY = YScale * Y;
		float scaledWidth = XScale * W1;
		float scaledHeight = YScale * height;
		float halfWidth = scaledWidth * 0.5f;
		float halfHeight = scaledHeight * 0.5f;

		float angleRadians = angle * 3.1415927f * 0.0055555557f;
		float cosAngle = cos(angleRadians);
		angle = sin(angleRadians);
		angleRadians = halfWidth + screenX;

		float CenterX = screenY + halfHeight;
		float CenterY = halfWidth * cosAngle;
		float rWidth = halfWidth * angle;
		float rHeight = angle * halfHeight;
		float cosHeight = halfHeight * cosAngle;
		float &zero = *game::CG_30060F10;

		float verts[8];
		verts[0] = -1.0f * CenterY + angleRadians - -1.0f * rWidth;
		verts[1] = -1.0f * cosHeight + -1.0f * rHeight + CenterX;
		verts[2] = 1.0f * CenterY + angleRadians - -1.0f * rWidth;
		verts[3] = -1.0f * cosHeight + 1.0f * rHeight + CenterX;
		verts[4] = 1.0f * CenterY + angleRadians - 1.0f * rWidth;
		verts[5] = 1.0f * cosHeight + 1.0f * rHeight + CenterX;
		verts[6] = -1.0f * CenterY + angleRadians - 1.0f * rWidth;
		verts[7] = 1.0f * cosHeight + -1.0f * rHeight + CenterX;

		(*game::cgame_mp::syscall)(game::CG_R_DRAWQUADPIC, verts, &zero, hShader );
	}

	void __declspec(naked) CG_DrawRotatedPic_asm()
	{
		__asm {

			push edx
			mov eax, [esp + 0x08]
			mov ecx, [esp + 0x0C]
			mov esi, [esp + 0x10]
			mov edi, [esp + 0x14]
			mov ebx, [esp + 0x18]

			push ebx
			push edi
			push esi
			push ecx
			push eax

			call CG_DrawRotatedPic
			add esp, 24
			ret
		}
	}

	static void UI_DrawHandlePic_Stub(float x, float y, float w, float h, game::qhandle_t hShader)
	{
		float s0, s1, t0, t1;
		float X1 = x;
		float W1 = w;

		if (w < 0)
		{
			w = -w;
			s0 = 1;
			s1 = 0;
		}
		else
		{
			s0 = 0;
			s1 = 1;
		}
		if (h < 0)
		{
			h = -h;
			t0 = 1;
			t1 = 0;
		}
		else
		{
			t0 = 0;
			t1 = 1;
		}

		if (cg_fixedAspect->integer && g_screenAspect > g_baseAspect)
		{
			if (w == 640 && h == 480)
			{
				auto BlackShader = (game::qhandle_t)(*game::ui_mp::syscall)(game::UI_R_REGISTERSHADERNOMIP, "black", 0);
				X1 = x - g_offset;
				W1 = w + g_offset * 2;
				hShader = BlackShader;
			}

			if ((w == 640 && h == 160) || (w == 640 && h == 320) || (w == 642 && h == 482))
			{
				X1 = x - (g_offset * 0.5f - 13);
				W1 = w + (g_offset - 26);
			}
		}

		SCR_AdjustFrom640(&X1, &y, &W1, &h);
		R_stretchpic_hook.invoke(X1, y, W1, h, s0, t0, s1, t1, hShader);
	}

	static void CG_DrawHudElemShader_Stub(float x, float y, float width, float h, game::qhandle_t hShader)
	{
		float X1 = x;
		float W1 = width;
		if (cg_fixedAspect->integer && g_screenAspect > g_baseAspect && (x == 0 && width == 640 ))
		{
			X1 = x - g_offset;
			W1 = width + g_offset * 2;
		}

		utils::hook::invoke<void>(ABSOLUTE_CGAME_MP(0x300192d0), X1, y, W1, h, hShader); // Draw normally
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			cg_fixedAspect = game::Cvar_Get("cg_fixedAspect", "1", game::CVAR_ARCHIVE);

			scheduler::loop(UpdateAspectGlobals, scheduler::pipeline::renderer);

			utils::hook::nop(0x416e36, 6); //remove flickering when loading into a map.
			utils::hook::nop(0x40a038, 6); //black shadow under console.

			utils::hook::jump(0x416810, SCR_AdjustFrom640);
			utils::hook::jump(0x4168d0, SCR_DrawPic);

			utils::hook::call(0x4dfbe1, R_stretchpic_bypass);

			R_stretchpic_hook.create(0x4ddd80, R_stretchpic_Stub);

		}

		void post_cgame() override
		{
			utils::hook::jump(ABSOLUTE_CGAME_MP(0x30019330), CG_DrawRotatedPic_asm);

			utils::hook::call(ABSOLUTE_CGAME_MP(0x300172dd), CG_DrawDamage_Stub);
			utils::hook::call(ABSOLUTE_CGAME_MP(0x30016b73), CG_DrawCrosshair_Stub);
			utils::hook::call(ABSOLUTE_CGAME_MP(0x3001fc1f), R_stretchpic_bypass);
			utils::hook::call(ABSOLUTE_CGAME_MP(0x3001f7ac), CG_DrawHudElemShader_Stub);
		}

		void post_ui_mp() override
		{
			utils::hook::jump(ABSOLUTE_UI_MP(0x40006e30), UI_DrawHandlePic_Stub);
		}
	};
}

//REGISTER_COMPONENT(widescreen::component)