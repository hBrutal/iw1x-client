#pragma once

namespace game
{
#define YAW 1
	
	typedef unsigned char byte;
	typedef enum { qfalse, qtrue } qboolean;
	typedef void (*xcommand_t)(void);
	
	typedef struct cvar_s
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
	} cvar_t;
}