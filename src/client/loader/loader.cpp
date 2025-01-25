#include <std_include.hpp>
#include "loader.hpp"
#include "tls.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>

FARPROC loader::load(const utils::nt::library& library, const std::string& buffer) const
{
	if (buffer.empty()) return nullptr;

	const utils::nt::library source(HMODULE(buffer.data()));
	if (!source) return nullptr;

	this->load_sections(library, source);
	this->load_imports(library, source);
	this->load_tls(library, source);

	DWORD old_protect;
	VirtualProtect(library.get_nt_headers(), 0x1000, PAGE_EXECUTE_READWRITE, &old_protect);

	library.get_optional_header()->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] = source.get_optional_header()->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	std::memmove(library.get_nt_headers(), source.get_nt_headers(), sizeof(IMAGE_NT_HEADERS) + source.get_nt_headers()->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER));

	return FARPROC(library.get_ptr() + source.get_relative_entry_point());
}

void loader::set_import_resolver(const std::function<void* (const std::string&, const std::string&)>& resolver)
{
	this->import_resolver_ = resolver;
}

void loader::load_section(const utils::nt::library& target, const utils::nt::library& source, IMAGE_SECTION_HEADER* section)
{
	void* target_ptr = target.get_ptr() + section->VirtualAddress;
	const void* source_ptr = source.get_ptr() + section->PointerToRawData;

	if (PBYTE(target_ptr) >= (target.get_ptr() + BINARY_PAYLOAD_SIZE))
	{
		throw std::runtime_error("Section exceeds the binary payload size, please increase it!");
	}
	
	if (section->SizeOfRawData > 0)
	{
#if 0
		std::memmove(target_ptr, source_ptr, section->SizeOfRawData);

		DWORD old_protect;
		VirtualProtect(target_ptr, section->Misc.VirtualSize, PAGE_EXECUTE_READWRITE, &old_protect);
#else
		const auto size_of_data = std::min(section->SizeOfRawData, section->Misc.VirtualSize);
		std::memmove(target_ptr, source_ptr, size_of_data);

		DWORD old_protect;
		VirtualProtect(target_ptr, size_of_data, PAGE_EXECUTE_READWRITE, &old_protect);
#endif
	}
}

void loader::load_sections(const utils::nt::library& target, const utils::nt::library& source) const
{
	for (auto& section : source.get_section_headers())
	{
		this->load_section(target, source, section);
	}
}

void loader::load_imports(const utils::nt::library& target, const utils::nt::library& source) const
{
	auto* const import_directory = &source.get_optional_header()->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	auto* descriptor = PIMAGE_IMPORT_DESCRIPTOR(target.get_ptr() + import_directory->VirtualAddress);

	while (descriptor->Name)
	{
		std::string name = LPSTR(target.get_ptr() + descriptor->Name);

		/*std::ostringstream oss;
		oss << "######## " << "name: " << name << "\n";
		std::string result = oss.str();
		OutputDebugString(result.c_str());*/

		auto* name_table_entry = reinterpret_cast<uintptr_t*>(target.get_ptr() + descriptor->OriginalFirstThunk);
		auto* address_table_entry = reinterpret_cast<uintptr_t*>(target.get_ptr() + descriptor->FirstThunk);

		if (!descriptor->OriginalFirstThunk)
		{
			name_table_entry = reinterpret_cast<uintptr_t*>(target.get_ptr() + descriptor->FirstThunk);
		}

		while (*name_table_entry)
		{
			FARPROC function = nullptr;
			std::string function_name;

			// is this an ordinal-only import?
			if (IMAGE_SNAP_BY_ORDINAL(*name_table_entry))
			{
				auto library = utils::nt::library::load(name);
				if (library)
				{
					function = GetProcAddress(library, MAKEINTRESOURCEA(IMAGE_ORDINAL(*name_table_entry)));
				}

				function_name = "#" + std::to_string(IMAGE_ORDINAL(*name_table_entry));
			}
			else
			{
				auto * import = PIMAGE_IMPORT_BY_NAME(target.get_ptr() + *name_table_entry);
				function_name = import->Name;

				if (this->import_resolver_) function = FARPROC(this->import_resolver_(name, function_name));
				if (!function)
				{
					auto library = utils::nt::library::load(name);
					if (library)
					{
						function = GetProcAddress(library, function_name.data());
					}
				}
			}

			if (!function)
			{
				throw std::runtime_error(utils::string::va("Unable to load import '%s' from library '%s'", function_name.data(), name.data()));
			}

			utils::hook::set(address_table_entry, reinterpret_cast<uintptr_t>(function));

			name_table_entry++;
			address_table_entry++;
		}

		descriptor++;
	}
}

void loader::load_tls(const utils::nt::library& target, const utils::nt::library& source) const
{
	if (source.get_optional_header()->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size)
	{
		auto* target_tls = tls::allocate_tls_index();
		auto* const source_tls = reinterpret_cast<PIMAGE_TLS_DIRECTORY>(target.get_ptr() + source.get_optional_header()
			->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);

		const auto tls_size = source_tls->EndAddressOfRawData - source_tls->StartAddressOfRawData;
		const auto tls_index = *reinterpret_cast<DWORD*>(target_tls->AddressOfIndex);
		utils::hook::set<DWORD>(source_tls->AddressOfIndex, tls_index);

		if (target_tls->AddressOfCallBacks)
		{
			utils::hook::set<void*>(target_tls->AddressOfCallBacks, nullptr);
		}

		DWORD old_protect;
		VirtualProtect(PVOID(target_tls->StartAddressOfRawData),
			source_tls->EndAddressOfRawData - source_tls->StartAddressOfRawData, PAGE_READWRITE,
			&old_protect);

		auto* const tls_base = *reinterpret_cast<LPVOID*>(__readfsdword(0x2C) + 4 * tls_index);
		std::memmove(tls_base, PVOID(source_tls->StartAddressOfRawData), tls_size);
		std::memmove(PVOID(target_tls->StartAddressOfRawData), PVOID(source_tls->StartAddressOfRawData), tls_size);

		VirtualProtect(target_tls, sizeof(*target_tls), PAGE_READWRITE, &old_protect);
		*target_tls = *source_tls;
	}
}