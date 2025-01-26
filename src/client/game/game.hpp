#pragma once

#include "launcher/launcher.hpp"

#define SELECT_VALUE(sp, mp) (game::environment::is_sp() ? (sp) : (mp))

namespace game
{
	namespace environment
	{
		launcher::mode get_mode();
		std::string get_binary();

		bool is_mp();
		bool is_sp();
		bool is_dedi();
		bool is_mohaa();

		void set_mode(launcher::mode mode);
		void set_mohaa();
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

		operator T*() const
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
}

#include "symbols.hpp"