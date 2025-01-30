#pragma once

namespace game
{
	enum qboolean
	{
		qfalse,
		qtrue
	};

	enum CvarFlags : std::uint32_t
	{
		CVAR_FLAG_NONE = 0,
		CVAR_FLAG_ARCHIVE = 0x1,
	};
	
	struct cvar_t
	{
		char* name;
		char* string;
		char* resetString;
		char* latchedString;
		int flags;
		qboolean modified;
		int modificationCount;
		float value;
		int integer;
		struct cvar_s* next;
		struct cvar_s* hashNext;
	};
}