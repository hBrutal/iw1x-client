#pragma once

namespace game
{
#define DotProduct( x,y )         ( ( x )[0] * ( y )[0] + ( x )[1] * ( y )[1] + ( x )[2] * ( y )[2] )
#define VectorCopy( a,b )         ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1],( b )[2] = ( a )[2] )
#define MAX_QPATH 64
#define M_PI        3.14159265358979323846f
#define VERSIONNUMBER "1.1"

	typedef unsigned char byte;
	typedef enum { qfalse, qtrue } qboolean;
	typedef void (*xcommand_t)(void);
	typedef struct vm_s vm_t;
	typedef int cvarHandle_t;
	typedef int qhandle_t;
	typedef float vec_t;
	typedef vec_t vec3_t[3];

	typedef enum
	{
		ERR_FATAL,
		ERR_DROP,
		// ...
	} errorParm_t;

	typedef enum
	{
		CA_DISCONNECTED = 0,
		CA_CONNECTING = 1,




		CA_ACTIVE = 6,
		// ...
	} connstate_t;

	typedef enum
	{
		EXEC_NOW,
		EXEC_INSERT,
		EXEC_APPEND
	} cbufExec_t;

	typedef enum
	{
		STAT_HEALTH,
		// ...
	} statIndex_t;

	typedef enum
	{
		/*
		these are taken from 1.5 I have commented out the one's that I haven't used yet.
		unsure if these are 100% correct as there are differences between versions
		*/
		// ...
		CG_PRINT = 0x0,
		CG_ERROR = 0x1,
		CG_GAMEMESSAGE = 0x2,
		//CG_BOLDGAMEMESSAGE = 0x3,
		//CG_DEATHMESSAGE = 0x4,
		//CG_SUBTITLE = 0x5,
		CG_MILLISECONDS = 0x6,
		//CG_CVAR_REGISTER = 0x7,
		//CG_CVAR_UPDATE = 0x8,
		//CG_CVAR_SET = 0x9,
		//CG_CVAR_SETVAR = 0xa,
		//CG_CVAR_VARIABLESTRINGBUFFER = 0xb,
		//CG_ARGC = 0xc,
		//CG_ARGV = 0xd,
		//CG_ARGS = 0xe,
		//CG_FS_FOPENFILE = 0xf,
		//CG_FS_READ = 0x10,
		//CG_FS_WRITE = 0x11,
		//CG_FS_FCLOSEFILE = 0x12,
		//CG_FS_GETFILELIST = 0x13,
		//CG_COM_SAVECVARSTOBUFFER = 0x14,
		//CG_COM_LOADCVARSFROMBUFFER = 0x15,
		//CG_SENDCONSOLECOMMAND = 0x16,
		//CG_ADDCOMMAND = 0x17,
		//CG_SENDCLIENTCOMMAND = 0x18,
		//CG_UPDATESCREEN = 0x19,
		//CG_DRAWNOTIFYLINES = 0x1a,
		//CG_DRAWBOLDMESSAGES = 0x1b,
		//CG_DRAWMINICONSOLE = 0x1c,
		//CG_DRAWSUBTITLES = 0x1d,
		//CG_DRAWSAY = 0x1e,
		//CG_CM_LOADMAP = 0x1f,
		//CG_CM_NUMINLINEMODELS = 0x20,
		//CG_CM_INLINEMODEL = 0x21,
		//CG_CM_TEMPBOXMODEL = 0x23,
		//CG_CM_POINTCONTENTS = 0x24,
		//CG_CM_TRANSFORMEDPOINTCONTENTS = 0x25,
		//CG_CM_BOXTRACE = 0x26,
		//CG_CM_TRANSFORMEDBOXTRACE = 0x27,
		//CG_CM_CAPSULETRACE = 0x28,
		//CG_CM_TRANSFORMEDCAPSULETRACE = 0x29,
		//CG_CM_TEMPCAPSULEMODEL = 0x2a,
		//CG_R_MARKFRAGMENTS = 0x2b,
		//CG_R_LOADWORLDMAP = 0x2c,
		//CG_R_REGISTERMODEL = 0x2d,
		//CG_R_FINISHLOADINGMODELS = 0x2e,
		CG_R_REGISTERSHADER = 0x30,
		//CG_R_GETXMODELBYHANDLE = 0x32,
		//CG_R_TEXT_WIDTH = 0x34,
		//CG_R_TEXT_HEIGHT = 0x35,
		//CG_R_TEXT_PAINT = 0x36,
		//CG_SE_TRANSLATEREFERENCE = 0x38,
		//CG_SE_LOCALIZEMESSAGE = 0x39,
		//CG_SE_PRINTSTRLEN = 0x3a,
		//CG_SE_READCHARFROMSTRING = 0x3b,
		//CG_R_CLEARSCENE = 0x3c,
		//CG_R_ADDREFENTITYTOSCENE = 0x3d,
		//CG_HUNKUSED = 0x3f,
		//CG_R_ADDPOLYTOSCENE = 0x40,
		//CG_R_ADDLIGHTTOSCENE = 0x42,
		//CG_R_SETFOG = 0x44,
		//CG_R_RENDERSCENE = 0x45,
		//CG_R_SAVESCREEN = 0x46,
		//CG_R_BLENDSAVEDSCREEN = 0x47,
		CG_R_SETCOLOR = 0x48,
		CG_R_DRAWSTRETCHPIC = 0x49,
		//CG_R_DRAWSTRETCHPICROTATE = 0x4b,
		CG_R_DRAWQUADPIC = 0x4c,
		//CG_R_MODELBOUNDS = 0x4d,
		//CG_GETGLCONFIG = 0x4e,
		//CG_GETGAMESTATE = 0x4f,
		//CG_GETCURRENTSNAPSHOTNUMBER = 0x50,
		//CG_GETSNAPSHOT = 0x51,
		//CG_GETSERVERCOMMAND = 0x52,
		//CG_GETCURRENTCMDNUMBER = 0x53,
		//CG_GETUSERCMD = 0x54,
		//CG_SETUSERCMDVALUE = 0x55,
		//CG_SETUSERCMDAIMVALUES = 0x56,
		//CG_SETUSERCMDINSHELLSHOCK = 0x57,
		CG_R_REGISTERSHADERNOMIP = 0x58,
		//CG_MEMORYREMAINING = 0x59,
		//CG_CL_LOOKUPCOLOR = 0x5e,
		//CG_PC_LOADSOURCE = 0x60,
		//CG_PC_FREESOURCE = 0x61,
		//CG_PC_READTOKEN = 0x62,
		//CG_PC_SOURCEFILEANDLINE = 0x63,
		//CG_SNAPVECTOR = 0x65,
		//CG_CIN_PLAYCINEMATIC = 0x67,
		//CG_CIN_STOPCINEMATIC = 0x68,
		//CG_CIN_RUNCINEMATIC = 0x69,
		//CG_CIN_DRAWCINEMATIC = 0x6a,
		//CG_CIN_SETEXTENTS = 0x6b,
		//CG_R_TRACKSTATISTICS = 0x6c,
		//CG_R_PICKSHADER = 0x6d,
		//CG_UI_LOADMENU = 0x7a,
		//CG_UI_POPUP = 0x7b,
		//CG_UI_CLOSEPOPUP = 0x7c,
		//CG_UI_CLOSEALLMENUS = 0x7d,
		//CG_UI_GETMAPDISPLAYNAME = 0x7e,
		//CG_UI_GETGAMETYPEDISPLAYNAME = 0x7f,
		//CG_CL_GETSERVERIPADDRESS = 0x80,
		//CG_XANIMPRECACHE = 0x81,
		//CG_XANIMCREATEANIMS = 0x82,
		//CG_XANIMCREATE = 0x83,
		//CG_XANIMCREATETREE = 0x84,
		//CG_XANIMBLEND = 0x85,
		//CG_XANIMCLEARGOALWEIGHT = 0x87,
		//CG_XANIMCLEARTREEGOALWEIGHTS = 0x88,
		//CG_XANIMCLEARTREEGOALWEIGHTSSTRICT = 0x89,
		//CG_XANIMSETCOMPLETEGOALWEIGHTKNOBALL = 0x8b,
		//CG_XANIMSETGOALWEIGHT = 0x8d,
		//CG_XANIMSETCOMPLETEGOALWEIGHT = 0x8e,
		//CG_XANIMSETANIMRATE = 0x8f,
		//CG_XANIMISLOOPED = 0x90,
		//CG_XANIMISLOOPING = 0x91,
		//CG_XANIMSETTIME = 0x92,
		//CG_XANIMGETTIME = 0x93,
		//CG_XANIMGETWEIGHT = 0x94,
		//CG_DOBJINVALIDATESKELS = 0x95,
		//CG_DOBJUPDATEINFO = 0x96,
		//CG_DOBJGETCLIENTNOTIFYLIST = 0x97,
		//CG_DOBJCALCANIM = 0x98,
		//CG_DOBJDISPLAYANIM = 0x99,
		//CG_XANIMCALCABSDELTA = 0x9b,
		//CG_XANIMGETRELDELTA = 0x9c,
		//CG_DOBJGETMATRIXARRAY = 0x9e,
		//CG_DOBJGETROTTRANSARRAY = 0x9f,
		//CG_DOBJSETROTTRANSINDEX = 0xa0,
		//CG_DOBJSETCONTROLROTTRANSINDEX = 0xa1,
		//CG_GETDOBJ = 0xa2,
		//CG_DOBJCREATE = 0xa5,
		//CG_SAFEDOBJFREE = 0xa6,
		//CG_DOBJCREATESKELFORBONE = 0xa9,
		//CG_DOBJCREATESKELFORBONES = 0xaa,
		//CG_DOBJGETHIERARCHYBITS = 0xab,
		//CG_DOBJCALCSKEL = 0xac,
		//CG_DOBJGETBONEINDEX = 0xaf,
		//CG_DOBJGETTREE = 0xb2,
		//CG_XANIMISPRIMITIVE = 0xb3,
		//CG_XANIMGETLENGTH = 0xb4,
		//CG_XANIMHASFINISHED = 0xb5,
		//CG_XANIMGETNUMCHILDREN = 0xb6,
		//CG_XANIMGETCHILDAT = 0xb7,
		//CG_XANIMGETANIMTREESIZE = 0xb8,
		//CG_XANIMCLONEANIMTREE = 0xb9,
		//CG_GETSTATSARRAY = 0xbc,
		//CG_Z_MALLOCINTERNAL = 0xbd,
		//CG_Z_FREEINTERNAL = 0xbe,
		//CG_COM_LOADSOUNDALIASES = 0xbf,
		//CG_COM_SOUNDALIASSTRING = 0xc0,
		//CG_COM_PICKSOUNDALIAS = 0xc1,
		//CG_MSS_PLAYSOUNDALIAS = 0xc3,
		//CG_MSS_PLAYBLENDEDSOUNDALIASES = 0xc4,
		//CG_SURFACETYPEFROMNAME = 0xc5,
		//CG_SURFACETYPETONAME = 0xc6,
		//CG_ADDDEBUGLINE = 0xC7,
		//CG_GETWEAPONINFOMEMORY = 0xc8,
		//CG_FREEWEAPONINFOMEMORY = 0xc9,
		//CG_HUNK_ALLOCINTERNAL = 0xca,
		//CG_HUNK_ALLOCLOWINTERNAL = 0xcb,
		//CG_HUNK_ALLOCLOWALIGNINTERNAL = 0xcd,
		//CG_GETNUMSCRIPTVARS = 0xce,
		//CG_GETNUMSCRIPTTHREADS = 0xcf
		//CG_GETSTRINGUSAGE = 0xd0,
		//CG_SETCLIENTLERPORIGIN = 0xd1,
		//CG_MSS_SETLISTENER = 0xd2,
		//CG_MSS_UPDATELOOPINGSOUNDS = 0xd3,
		//CG_MSS_STOPSOUNDS = 0xd4,
		//CG_MSS_PLAYMUSICALIAS = 0xd5,
		//CG_MSS_STOPMUSIC = 0xd6,
		//CG_MSS_PLAYAMBIENTALIAS = 0xd7,
		//CG_MSS_FADEALLSOUNDS = 0xd8,
		//CG_MSS_FADESELECTSOUNDS = 0xd9,
		//CG_MSS_SETENVIRONMENTEFFECTS = 0xda,
		//CG_MSS_GETSOUNDOVERLAY = 0xdb,
		//CG_FX_REGISTEREFFECT = 0xdf,
		//CG_FX_GETBONEINDEX = 0xe0,
		//CG_FX_PLAYSIMPLEEFFECTID = 0xe4,
		//CG_FX_PLAYEFFECTID = 0xe5,
		//CG_FX_PLAYENTITYEFFECTID = 0xe6,
		//CG_FX_ADDSCHEDULEDEFFECTS = 0xe7,
		//CG_FX_INITSYSTEM = 0xe8,
		//CG_FX_FREESYSTEM = 0xe9,
		//CG_FX_FREEACTIVE = 0xea,
		//CG_FX_ADJUSTTIME = 0xeb,
		//CG_FX_ADJUSTCAMERA = 0xec,
		//CG_CAPTURNRATE = 0xf1,
		//CG_SETVIEWANGLES = 0xf2,
		//CG_SYNCTIMES = 0xf3,
		//CG_DATETIMESTAMP = 0xf4,
		//CG_EXECUTESTRING = 0xf5
		// ...
	} cgameImport_t;

	typedef enum
	{
		/*
			these are taken from 1.5 I have commented out the one's that I haven't used yet.
			unsure if these are 100% correct as there are differences between versions
		*/

		// ...
		//UI_ERROR = 0X0,
		//UI_PRINT = 0x1,
		//UI_GETLANGUAGENAME = 0x2,
		//UI_VERIFYLANGUAGESELECTION = 0X3,
		//UI_MILLISECONDS = 0x4,
		//UI_CVAR_REGISTER = 0X5,
		//UI_CVAR_UPDATE = 0X6,
		//UI_CVAR_SET = 0X7,
		//UI_CVAR_VARIABLEVALUE = 0X8,
		//UI_CVAR_VARIABLESTRINGBUFFER = 0X9,
		//UI_CVAR_SETVALUE = 0XA,
		//UI_ARGV = 0XF,
		//UI_CMD_EXECUTETEXT = 0X10,
		//UI_FS_FOPENFILE = 0x11,
		//UI_FS_READ = 0x12,
		//UI_FS_FCLOSEFILE = 0x15,
		//UI_FS_GETFILELIST = 0x16,
		UI_R_REGISTERSHADERNOMIP = 0x19,
		//UI_R_SETCOLOR = 0x21,
		//UI_R_DRAWSTRETCHPIC = 0x22,
		//UI_COM_SOUNDALIASSTRING = 0X26,
		//UI_MSS_PLAYLOCALSOUNDALIAS = 0X27,
		//UI_KEY_CLEARSTATES = 0x2F,
		//UI_KEY_GETCATCHER = 0x30,
		//UI_KET_SETCATCHER = 0x31,
		//UI_GETCLIENTSTATE = 0x33,
		//UI_GETGLCONFIG = 0x34,
		//UI_GETGLCONFIGSTRING = 0x35,
		//UI_GETCLIENTNAME = 0X36,
		//UI_SETCDKEY = 0X41,
		//UI_R_REGISTERFONT = 0x42,
		//UI_R_TEXTWIDTH = 0x43,
		//UI_R_TEXTPAINT = 0x45,
		//UI_SE_TRANSLATEREFERENCE = 0X47,
		//UI_SE_LOCALIZEMESSAGE = 0x48,
		//UI_PC_LOADSOURCE = 0x4A,
		//UI_PC_FREESOURCE = 0x4B,
		//UI_PC_READTOKEN = 0x4C,
		//UI_PC_SOURCEFILEANDLINE = 0X4D,
		//UI_REALTIME = 0x4E,
		//UI_LAN_GETSERVERCOUNT = 0X4F,
		//UI_LAN_WAITSERVERRESPONSE = 0X50,
		//UI_LAN_GETSERVERADDRESSSTRING = 0X51,
		//UI_LAN_GETSERVERINFO = 0X52,
		//UI_LAN_MARKSERVERDIRTY = 0X53,
		//UI_LAN_UPDATEDIRTYPINGS = 0X54,
		//UI_LAN_RESETPINGS = 0X55,
		//UI_LAN_LOADCACHEDSERVERS = 0X56,
		//UI_LAN_SAVECACHEDSERVERS = 0X57,
		//UI_LAN_ADDSERVER = 0X58,
		//UI_LAN_REMOVESERVER = 0X59,
		//UI_CIN_PLAYCINEMATIC = 0X5A,
		//UI_CIN_STOPCINEMATIC = 0X5B,
		//UI_CIN_RUNCINEMATIC = 0X5C,
		//UI_CIN_DRAWCINEMATIC = 0X5D,
		//UI_CIN_SETEXTENTS = 0X5E,
		//UI_VERIFYCDKEY = 0X5F,
		//UI_LAN_SERVERSTATUS = 0X60,
		//UI_LAN_GETSERVERPING = 0X61,
		//UI_LAN_SERVERISDIRTY = 0X63,
		//UI_LAN_COMPARESERVERS = 0X64,
		// ...
	} uiImport_t;

	typedef enum
	{
		// ...
	} stereoFrame_t;

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

	typedef struct shader_s {
		char name[MAX_QPATH];
	} shader_t;

	typedef struct
	{
		cvarHandle_t handle;
		int modificationCount;
		float value;
		int integer;
		// ...
	} vmCvar_t;

	typedef struct
	{
		vmCvar_t* vmCvar;
		char* cvarName;
		char* defaultString;
		int cvarFlags;
	} cvarTable_t;

	typedef struct playerState_s
	{
		byte gap_0x0[0xB0];
		int weapon; // 0xB0
		byte gap_0xB4[0x40];
		statIndex_t stats[6]; // 0xf4
		// ...
	} playerState_t;

	typedef struct
	{
		playerState_t* ps;
		// ...
	} pmove_t;

	typedef struct weaponInfo_s
	{
		byte gap_0x0[0x4];              // 0x000
		char* weapon_name;              // 0x004
		char* displayName;              // 0x008
		char* AIOverlayDescription;     // 0x00C
		char* gunModel;                 // 0x010
		char* handModel;                // 0x014
		byte gap_0x18[0x4];             // 0x018
		char* idleAnim;                 // 0x01C
		byte gap_0x20[0x4];             // 0x020
		char* fireAnim;                 // 0x024
		byte gap_0x28[0x1FC];           // 0x028 - 0x223
		char* adsOverlayShader;			// = 0x224 | 0x89 * 4
		int adsOverlayReticle;			// 0x228
		float adsOverlayWidth;			// 0x22C
		float adsOverlayHeight;			// 0x22C
		// ...
	} weaponInfo_t;

	typedef enum
	{
		// ...
		NA_LOOPBACK = 2,
		// ...
	} netadrtype_t;

	typedef struct
	{
		netadrtype_t type;
		byte ip[4];
		byte ipx[10];
		unsigned short port;
	} netadr_t;

	/*typedef struct
	{
		float x;
		float y;
		float w;
		float h;
	} rectDef_t;
	typedef rectDef_t Rectangle;
	typedef struct
	{
		Rectangle rect;
		Rectangle rectClient;
		const char* name;
		// ...
	} windowDef_t;

	typedef windowDef_t Window;
	typedef struct itemDef_s
	{
		Window window;
		// ...
	} itemDef_t;*/
}