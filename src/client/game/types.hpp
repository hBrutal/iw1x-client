#pragma once

namespace game
{
#define CVAR_ARCHIVE 1
#define MAX_WEAPONS 64
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
		// ...
	} cvar_t;
	
	typedef struct playerState_s
	{
		byte gap_0x0[0xB0];
		int weapon; // 0xB0
		// ...
	} playerState_t;
	
	typedef struct
	{
		playerState_t* ps;
		// ...
	} pmove_t;
	
	typedef struct weaponInfo_s
	{
		byte gap_0x0[0x224];
		char* adsOverlayShader;	// 0x224
		// ...
	} weaponInfo_t;
}