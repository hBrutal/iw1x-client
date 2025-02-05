#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "loader/loader.hpp"
#include <utils/io.hpp>
#include <utils/string.hpp>

#include <DbgHelp.h>

static void remove_crash_file()
{
    utils::io::remove_file("__codmp");
    utils::io::remove_file("__cosp");
}

[[noreturn]] static void WINAPI exit_hook(const int code)
{
    component_loader::pre_destroy();
    std::exit(code);
}

static BOOL WINAPI system_parameters_info_a(const UINT uiAction, const UINT uiParam, const PVOID pvParam, const UINT fWinIni)
{
    component_loader::post_unpack();
    return SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);
}

static FARPROC WINAPI get_proc_address(const HMODULE hModule, const LPCSTR lpProcName)
{
    if (lpProcName == "GlobalMemoryStatusEx"s)
        component_loader::post_unpack();
    return GetProcAddress(hModule, lpProcName);
}

static FARPROC load_binary()
{
    loader loader;
    utils::nt::library self;

    loader.set_import_resolver([self](const std::string& library, const std::string& function) -> void*
        {
            if (function == "ExitProcess")
                return exit_hook;
            if (function == "SystemParametersInfoA")
                return system_parameters_info_a;
            if (function == "GetProcAddress")
                return get_proc_address;
            
            return component_loader::load_import(library, function);
        });

    // Check if the CoD file is named mohaa
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, sizeof(path));
    std::filesystem::path pathFs = path;
    pathFs.replace_filename("mohaa.exe");
    if (utils::io::file_exists(pathFs.string()))
        game::environment::set_mohaa();

    auto client_filename = game::environment::get_client_filename();

    std::string data;
    if (!utils::io::read_file(client_filename, &data))
        throw std::runtime_error(utils::string::va("Failed to read %s\nIs cod-mod in your Call of Duty folder?", client_filename.data()));

    return loader.load(self, data);
}

static void enable_dpi_awareness()
{
    const utils::nt::library user32{ "user32.dll" };
    const auto set_dpi = user32 ? user32.get_proc<BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT)>("SetProcessDpiAwarenessContext") : nullptr;
    if (set_dpi)
        set_dpi(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
    //SetProcessDEPPolicy(PROCESS_DEP_ENABLE);
    enable_dpi_awareness();

#ifdef DEBUG
    remove_crash_file();
#endif

    auto premature_shutdown = true;
    const auto _ = gsl::finally([&premature_shutdown]()
        {
            if (premature_shutdown)
                component_loader::pre_destroy();
        });

    FARPROC entry_point;
    try
    {
        if (!component_loader::post_start())
            return 1;

        entry_point = load_binary();
        if (!entry_point)
            throw std::runtime_error("Unable to load binary into memory");
        
        if (!component_loader::post_load())
            return 1;
    }
    catch (std::exception& ex)
    {
        MSG_BOX_ERROR(ex.what());
        return 1;
    }

    return static_cast<int>(entry_point());
}