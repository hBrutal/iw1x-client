#pragma once

#include "structs.hpp"
#include "launcher/launcher.hpp"

extern DWORD address_cgame_mp;
extern DWORD address_ui_mp;

#define OFFSET_CGAME_MP 0x30000000
#define OFFSET_UI_MP	0x40000000

#define ABSOLUTE_CGAME_MP(relative) (address_cgame_mp + (relative - OFFSET_CGAME_MP))
#define ABSOLUTE_UI_MP(relative)	(address_ui_mp + (relative - OFFSET_UI_MP))

#define SP_OR_MP(sp, mp) (game::environment::is_sp() ? (sp) : (mp))

namespace game
{
	namespace environment
	{
		launcher::mode get_mode();
		std::string get_client_filename();

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
		symbol(const size_t sp_address, const size_t mp_address, const ptrdiff_t offset = 0) :
			sp_object(reinterpret_cast<T*>(sp_address)),
			mp_object(reinterpret_cast<T*>(mp_address)),
			offset(offset)
		{
		}

		T* get() const
		{
			T* ptr = environment::is_sp() ? sp_object : mp_object;
			uintptr_t base_address = 0;

			switch (offset)
			{
			case OFFSET_CGAME_MP:
				base_address = address_cgame_mp;
				break;
			case OFFSET_UI_MP:
				base_address = address_ui_mp;
				break;
			default:
				return ptr;
			}

			return reinterpret_cast<T*>(base_address + (reinterpret_cast<uintptr_t>(ptr) - offset));
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
		T* sp_object;
		T* mp_object;
		ptrdiff_t offset;
	};
	
	int Cmd_Argc();
	char* Cmd_Argv(int arg);
}

#include "symbols.hpp"