#pragma once

#include "structs.hpp"

namespace game
{
	namespace environment
	{
		bool is_mp();
		bool is_sp();

		std::string get_string();
	}

	template <typename T>
	class symbol
	{
	public:
		symbol(const size_t sp_address, const size_t mp_address)
			: sp_object_(reinterpret_cast<T*>(sp_address))
			, mp_object_(reinterpret_cast<T*>(mp_address))
		{
		}

		T* get() const
		{
			if (environment::is_sp())
			{
				return sp_object_;
			}

			return mp_object_;
		}

		operator T* () const
		{
			return this->get();
		}

		T* operator->() const
		{
			return this->get();
		}

	private:
		T* sp_object_;
		T* mp_object_;
	};

	[[nodiscard]] int Cmd_Argc();
	[[nodiscard]] const char* Cmd_Argv(int index);

	[[nodiscard]] int SV_Cmd_Argc();
	[[nodiscard]] const char* SV_Cmd_Argv(int index);

	[[nodiscard]] HANDLE Sys_OpenFileReliable(const char* filename);

	[[nodiscard]] bool is_headless();
	void show_error(const std::string& text, const std::string& title = "Error");

	constexpr auto MAX_GENTITIES = 2048;
	constexpr auto ENTITYNUM_NONE = MAX_GENTITIES - 1;

	constexpr auto JUMP_LAND_SLOWDOWN_TIME = 1800;
}

#include "symbols.hpp"
