#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include <utils/hook.hpp>

namespace net
{
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// change the min/max value of cl_maxpackets
			utils::hook::set(0x0040b9df + 2, (BYTE)0x19);
			utils::hook::set(0x0040b9e4 + 1, "25");

			utils::hook::set(0x0040b9eb + 2, (BYTE)0x7D);
			utils::hook::set(0x0040b9f0 + 1, "125");
		}
	};
}

REGISTER_COMPONENT(net::component)