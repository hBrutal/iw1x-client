#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "launcher/launcher.hpp"
#include "loader/loader.hpp"
#include <utils/flags.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

#include <DbgHelp.h>

#include <version.hpp>

const char* get_current_date()
{
    auto now = std::chrono::system_clock::now();
    auto current_time = std::chrono::system_clock::to_time_t(now);
    std::tm local_time{};

    (void)localtime_s(&local_time, &current_time);

    std::stringstream ss;
    ss << std::put_time(&local_time, "%Y%m%d_%H%M%S");

    const auto result = ss.str();
    return utils::string::va("%s", result.data());
}

void remove_crash_file()
{
    utils::io::remove_file("__codmp");
    utils::io::remove_file("__cosp");
}

LONG WINAPI exception_handler(PEXCEPTION_POINTERS exception_info)
{
    if (exception_info->ExceptionRecord->ExceptionCode == 0x406D1388)
    {
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    if (exception_info->ExceptionRecord->ExceptionCode < 0x80000000 ||
        exception_info->ExceptionRecord->ExceptionCode == 0xE06D7363)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    MINIDUMP_EXCEPTION_INFORMATION exception_information =
    {
        GetCurrentThreadId(), exception_info, FALSE
    };

    const auto type = MiniDumpIgnoreInaccessibleMemory
        | MiniDumpWithHandleData
        | MiniDumpScanMemory
        | MiniDumpWithProcessThreadData
        | MiniDumpWithFullMemoryInfo
        | MiniDumpWithThreadInfo;

    CreateDirectoryA("minidumps", nullptr);
    const auto* file_name = utils::string::va("minidumps\\cod-mod_%s_%s.dmp", SHORTVERSION, get_current_date());
    constexpr auto file_share = FILE_SHARE_READ | FILE_SHARE_WRITE;

    const auto file_handle = CreateFileA(file_name, GENERIC_WRITE | GENERIC_READ, file_share, nullptr, CREATE_ALWAYS, NULL, nullptr);

    if (!MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
        file_handle, static_cast<MINIDUMP_TYPE>(type),
        &exception_information, nullptr, nullptr))
    {
        char buf[4096]{};
        sprintf_s(buf, "An exception 0x%08X occurred at location 0x%p\n",
            exception_info->ExceptionRecord->ExceptionCode,
            exception_info->ExceptionRecord->ExceptionAddress);
        MSG_BOX_ERROR(buf);
    }

    CloseHandle(file_handle);
    TerminateProcess(GetCurrentProcess(), exception_info->ExceptionRecord->ExceptionCode);

    return EXCEPTION_CONTINUE_SEARCH;
}

[[noreturn]] void WINAPI exit_hook(const int code)
{
    component_loader::pre_destroy();
    std::exit(code);
}

BOOL WINAPI system_parameters_info_a(const UINT uiAction, const UINT uiParam, const PVOID pvParam, const UINT fWinIni)
{
    component_loader::post_unpack();
    return SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);
}

FARPROC WINAPI get_proc_address(const HMODULE hModule, const LPCSTR lpProcName)
{
    if (lpProcName == "GlobalMemoryStatusEx"s)
    {
        component_loader::post_unpack();
    }

    return GetProcAddress(hModule, lpProcName);
}

launcher::mode detect_mode_from_arguments()
{
    if (utils::flags::has_flag("dedicated"))
    {
        return launcher::mode::server;
    }

    if (utils::flags::has_flag("multiplayer"))
    {
        return launcher::mode::multiplayer;
    }

    if (utils::flags::has_flag("singleplayer"))
    {
        return launcher::mode::singleplayer;
    }

    return launcher::mode::none;
}

FARPROC load_binary()
{
    loader loader;
    utils::nt::library self;

    loader.set_import_resolver([self](const std::string& library, const std::string& function) -> void*
        {
            if (function == "ExitProcess")
            {
                return exit_hook;
            }

            if (function == "SystemParametersInfoA")
            {
                return system_parameters_info_a;
            }

            if (function == "GetProcAddress")
            {
                return get_proc_address;
            }

            return component_loader::load_import(library, function);
        });

    //// Check if the CoD file is named mohaa
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, sizeof(path));
    std::filesystem::path pathFs = path;
    pathFs.replace_filename("mohaa.exe");
    if (utils::io::file_exists(pathFs.string()))
        game::environment::set_mohaa();
    ////

    auto client_filename = game::environment::get_client_filename();

    std::string data;
    if (!utils::io::read_file(client_filename, &data))
        throw std::runtime_error(utils::string::va("Failed to read %s\nIs cod-mod into your Call of Duty folder?", client_filename.data()));

#ifdef DEBUG
    remove_crash_file();
#endif

    return loader.load(self, data);
}

void enable_dpi_awareness()
{
    const utils::nt::library user32{ "user32.dll" };
    const auto set_dpi = user32 ? user32.get_proc<BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT)>("SetProcessDpiAwarenessContext") : nullptr;
    if (set_dpi)
        set_dpi(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
    AddVectoredExceptionHandler(0, exception_handler);
    SetProcessDEPPolicy(PROCESS_DEP_ENABLE);
    enable_dpi_awareness();

    auto premature_shutdown = true;
    const auto _ = gsl::finally([&premature_shutdown]()
        {
            if (premature_shutdown)
            {
                component_loader::pre_destroy();
            }
        });

    FARPROC entry_point;
    try
    {
        if (!component_loader::post_start())
            return 1;

        auto mode = detect_mode_from_arguments();
        if (mode == launcher::mode::none)
        {
            const launcher launcher;
            mode = launcher.run();
            if (mode == launcher::mode::none)
                return 1;
            game::environment::set_mode(mode);
        }
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