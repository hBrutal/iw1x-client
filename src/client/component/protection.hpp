#pragma once

namespace protection
{
	extern utils::hook::detour CG_ServerCommand_hook;
	extern void CG_ServerCommand_stub();
}