#pragma once

#include "game/game.hpp"
#include <utils/string.hpp>

namespace window
{
	extern char sys_cmdline[game::MAX_STRING_CHARS];
	void MSG(const std::string& text, UINT flags = MB_OK);
}