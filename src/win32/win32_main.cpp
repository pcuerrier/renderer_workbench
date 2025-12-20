#include "win32/win32_main.h"
#include "win32/win32_input.h"

#include "utils/handmade_math.h"

#include "core/memory.h"

#include "renderer/renderer.h"
#include "app/app.h"

#define APP_NAME "handmade-renderer"
#define LAPP_NAME L"handmade-renderer"

global bool g_running;
global LONG g_window_style;
global LONG g_window_ex_style;
global RECT g_windowed_rect;
global bool g_fullscreen = false;
global bool g_show_debug_info = true;

global Win32AppPerfData g_perf_data;
global Win32Framebuffer g_framebuffer;

internal HWND Win32_CreateMainGameWindow(_In_ HINSTANCE instance);
internal BOOL Win32_AppIsAlreadyRunning();
internal void Win32_BlitDIBSection(HWND window);
internal void Win32_ResizeDIBSection(i32 width, i32 height);
internal void Win32_ProcessPendingMessages(HWND window, Input& input);
internal LARGE_INTEGER Win32_GetWallClock();
internal i64 Win32_GetMicroSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end);

internal LRESULT CALLBACK Win32_WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (message)
    {
        case WM_QUIT:
        case WM_DESTROY:
        case WM_CLOSE:
        {
            g_running = false;
        } break;

        case WM_SIZE:
        {
            RECT client_rect;
            GetClientRect(window, &client_rect);
            i32 width = client_rect.right - client_rect.left;
            i32 height = client_rect.bottom - client_rect.top;
            //Win32_ResizeDIBSection(APP_RES_WIDTH, APP_RES_HEIGHT);
            Renderer_Resize(width, height);
        } break;

        default:
        {
            result = DefWindowProc(window, message, wParam, lParam);
        } break;
    }
    return result;
}


i32 __stdcall WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, i32 show_cmd)
{
    UNREFERENCED_PARAMETER(prev_instance);
    UNREFERENCED_PARAMETER(cmd_line);
    UNREFERENCED_PARAMETER(show_cmd);

    HMODULE nt_dll_module_handle;

    if ((nt_dll_module_handle = GetModuleHandleA("ntdll.dll")) == NULL)
    {
        MessageBoxA(NULL, "Couldn't load ntdll.dll!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        return 1;
    }

    if ((NtQueryTimerResolution = (_NtQueryTimerResolution)((void*)(GetProcAddress(nt_dll_module_handle, "NtQueryTimerResolution")))) == NULL)
    {
        MessageBoxA(NULL, "Couldn't find the NtQueryTimerResolution function in ntdll.dll!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        return 1;
    }

    if (Win32_AppIsAlreadyRunning())
    {
        MessageBoxA(NULL, "Game is already running", "Error", MB_OK | MB_ICONEXCLAMATION);
        return 1;
    }

    QueryPerformanceFrequency((LARGE_INTEGER*)&g_perf_data.perf_frenquency);

    UINT desired_scheduler_ms = 1;
    bool sleep_is_granular = (timeBeginPeriod(desired_scheduler_ms) == TIMERR_NOERROR);

    if (SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS) == 0)
    {
        MessageBoxA(NULL, "Failed to set process priority", "Error", MB_OK | MB_ICONEXCLAMATION);
        return 1;
    }
    if (SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST) == 0)
    {
        MessageBoxA(NULL, "Failed to set thread priority", "Error", MB_OK | MB_ICONEXCLAMATION);
        return 1;
    }

    Win32LoadXInput();

    HWND window = Win32_CreateMainGameWindow(instance);
    if (!window)
    {
        return 1;
    }

    g_running = true;
    Input old_input = {};
    Input new_input = {};
#if DEBUG
    LPVOID base_address = (void*)Terabytes(2);
#else
    LPVOID base_address = 0;
#endif

    Memory app_memory = {};
    app_memory.permanent_storageSize = Megabytes(64);
    app_memory.transient_storageSize = Gigabytes(4);

    size_t totalSize = (size_t)(app_memory.permanent_storageSize + app_memory.transient_storageSize);
    app_memory.permanent_storage = VirtualAlloc(base_address, totalSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    app_memory.transient_storage = (u8*)app_memory.permanent_storage + app_memory.permanent_storageSize;

    NtQueryTimerResolution(&g_perf_data.minimum_timer_resolution, &g_perf_data.maximum_timer_resolution, &g_perf_data.current_timer_resolution);
    GetSystemInfo(&g_perf_data.system_info);
    GetSystemTimeAsFileTime((FILETIME*)&g_perf_data.previous_system_time);

    if (app_memory.permanent_storage && app_memory.transient_storage)
    {
        i64 elapsed_micro_seconds_accumulator = 0;
        i64 elapsed_micro_seconds_accumulator_cooked = 0;

        u64 elapsed_cycles_accumulator = 0;
        u64 elapsed_cycles_accumulator_cooked = 0;

        LARGE_INTEGER frame_start = Win32_GetWallClock();
        u64 cycle_count_start = __rdtsc();

        Renderer_Init(window); // Initialize OpenGL context

        ShowWindow(window, SW_SHOW);

        while (g_running)
        {
            old_input = new_input;
            new_input.mouse.wheel_value = 0;
            Win32_ProcessPendingMessages(window, new_input);

            //GameUpdateAndRender(app_memory, g_framebuffer.buffer, new_input, old_input, asset_store, TARGET_SECONDS_PER_FRAME);
            RenderQueue render_queue = AppUpdate(); // Fill Render

            // Renderer code
            Renderer_ClearScreen(0.2f, 0.3f, 0.3f, 1.0f);
            for (size_t i = 0; i < render_queue.command_count; ++i)
            {
                Renderer_Draw(&render_queue.commands[i]);
            }

            Renderer_Present();
            SwapBuffers(GetDC(window));

            //Win32_BlitDIBSection(window);
            ++g_perf_data.total_frame_rendered;

            LARGE_INTEGER frame_end = Win32_GetWallClock();
            u64 cycle_count_end = __rdtsc();

            i64 elapsed_micro_seconds = Win32_GetMicroSecondsElapsed(frame_start, frame_end);
            elapsed_micro_seconds_accumulator += elapsed_micro_seconds;

            u64 elapsed_cycles = cycle_count_end - cycle_count_start;
            elapsed_cycles_accumulator += elapsed_cycles;

            if (elapsed_micro_seconds < TARGET_MICROSECONDS_PER_FRAME)
            {
                while (elapsed_micro_seconds < TARGET_MICROSECONDS_PER_FRAME)
                {
                    if (elapsed_micro_seconds < RoundFloatToInt(((f32)TARGET_MICROSECONDS_PER_FRAME) * 0.85f))
                    {
                        Sleep(1); // Could be anywhere from 1ms to a full system timer tick? (~15.625ms)
                    }
                    frame_end = Win32_GetWallClock();
                    elapsed_micro_seconds = Win32_GetMicroSecondsElapsed(frame_start, frame_end);
                }
            }
            else
            {
                OutputDebugStringA("Frame took too long!\n");
            }
            elapsed_micro_seconds_accumulator_cooked += elapsed_micro_seconds;

            elapsed_cycles = __rdtsc() - cycle_count_start;
            elapsed_cycles_accumulator_cooked += elapsed_cycles;

            if ((g_perf_data.total_frame_rendered % CALCULATE_PERF_TIME_EVERY_X_FRAMES) == 0)
            {
                g_perf_data.ms_raw.avg = (f32)elapsed_micro_seconds_accumulator / (f32)CALCULATE_PERF_TIME_EVERY_X_FRAMES * 0.001f;
                g_perf_data.fps_raw.avg = 1.0f / (g_perf_data.ms_raw.avg * 0.001f);
                g_perf_data.cycles_raw.avg = RoundFloatToUInt((f32)elapsed_cycles_accumulator / (f32)CALCULATE_PERF_TIME_EVERY_X_FRAMES);

                g_perf_data.ms_cooked.avg = (f32)elapsed_micro_seconds_accumulator_cooked / (f32)CALCULATE_PERF_TIME_EVERY_X_FRAMES * 0.001f;
                g_perf_data.fps_cooked.avg = 1.0f / (g_perf_data.ms_cooked.avg * 0.001f);
                g_perf_data.cycles_cooked.avg = RoundFloatToUInt((f32)elapsed_cycles_accumulator_cooked / (f32)CALCULATE_PERF_TIME_EVERY_X_FRAMES);

                g_perf_data.fps_raw.min    = 1.0f / (g_perf_data.ms_raw.max * 0.001f);
                g_perf_data.fps_raw.max    = 1.0f / (g_perf_data.ms_raw.min * 0.001f);
                g_perf_data.fps_cooked.min = 1.0f / (g_perf_data.ms_cooked.max * 0.001f);
                g_perf_data.fps_cooked.max = 1.0f / (g_perf_data.ms_cooked.min * 0.001f);

                elapsed_micro_seconds_accumulator = 0;
                elapsed_micro_seconds_accumulator_cooked = 0;
                elapsed_cycles_accumulator = 0;
                elapsed_cycles_accumulator_cooked = 0;

                GetProcessHandleCount(GetCurrentProcess(), &g_perf_data.handle_count);
                GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&g_perf_data.mem_info, sizeof(g_perf_data.mem_info));

                GetSystemTimeAsFileTime((FILETIME*)&g_perf_data.current_system_time);

                GetProcessTimes(GetCurrentProcess(), 
                    &g_perf_data.process_creation_time, 
                    &g_perf_data.process_exit_time, 
                    (FILETIME*)&g_perf_data.current_kernel_cpu_time,
                    (FILETIME*)&g_perf_data.current_user_cpu_time);

                g_perf_data.cpu_percent = (f64)((g_perf_data.current_kernel_cpu_time - g_perf_data.previous_kernel_cpu_time) + \
                    (g_perf_data.current_user_cpu_time - g_perf_data.previous_user_cpu_time));

                g_perf_data.cpu_percent /= (f64)(g_perf_data.current_system_time - g_perf_data.previous_system_time);

                g_perf_data.cpu_percent /= (f64)g_perf_data.system_info.dwNumberOfProcessors;

                g_perf_data.cpu_percent *= 100.0;

                g_perf_data.previous_kernel_cpu_time = g_perf_data.current_kernel_cpu_time;
                g_perf_data.previous_user_cpu_time = g_perf_data.current_user_cpu_time;
                g_perf_data.previous_system_time = g_perf_data.current_system_time;
            }

            if (g_show_debug_info)
            {
                char debug_text_buffer[256] = {};
                
                //sprintf_s(debug_text_buffer, sizeof(debug_text_buffer),
                //"ms : %.01f/%.01f\n", g_perf_data.ms_cooked.avg, g_perf_data.ms_raw.avg);
                //OutputDebugStringA(debug_text_buffer);
                //DrawString(g_framebuffer.buffer, asset_store.sprites[1], debug_text_buffer, 0.0f, 0.0f, {0, 0, 0, 255});
                
                sprintf_s(debug_text_buffer, sizeof(debug_text_buffer),
                "FPS : %.01f/%.01f\n", g_perf_data.fps_cooked.avg, g_perf_data.fps_raw.avg);
                OutputDebugStringA(debug_text_buffer);
                //DrawString(g_framebuffer.buffer, asset_store.sprites[1], debug_text_buffer, 0.0f, 10.0f, {0, 0, 0, 255});
                
                //sprintf_s(debug_text_buffer, sizeof(debug_text_buffer), "Handles: %lu", g_perf_data.handle_count);
                //DrawString(g_framebuffer.buffer, asset_store.sprites[1], debug_text_buffer, 0.0f, 20.0f, {0, 0, 0, 255});
                
                //sprintf_s(debug_text_buffer, sizeof(debug_text_buffer), "Memory: %0.01f MB", (f32)g_perf_data.mem_info.PrivateUsage / (1024.0f * 1024.0f));
                //DrawString(g_framebuffer.buffer, asset_store.sprites[1], debug_text_buffer, 0.0f, 30.0f, {0, 0, 0, 255});
                
                //sprintf_s(debug_text_buffer, sizeof(debug_text_buffer), "CPU Usage: %0.02f%%", (f32)g_perf_data.cpu_percent);
                //DrawString(g_framebuffer.buffer, asset_store.sprites[1], debug_text_buffer, 0.0f, 40.0f, {0, 0, 0, 255});
                }

            frame_start = frame_end;
        }
    }

    return 0;
}

internal LARGE_INTEGER Win32_GetWallClock()
{
    LARGE_INTEGER result;
    QueryPerformanceCounter(&result);
    return result;
}

internal i64 Win32_GetMicroSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
    i64 elapsed = (end.QuadPart - start.QuadPart) * 1000000 / g_perf_data.perf_frenquency;
    return elapsed;
}

internal void Win32_ResizeDIBSection(i32 width, i32 height)
{
    Framebuffer& framebuffer = g_framebuffer.buffer;
    if (framebuffer.memory)
    {
        VirtualFree(framebuffer.memory, 0, MEM_RELEASE);
    }

    framebuffer.bytes_per_pixel = APP_BPP / 8;
    framebuffer.width = width;
    framebuffer.height = height;
    framebuffer.pitch = width * framebuffer.bytes_per_pixel;

    g_framebuffer.bitmap_info.bmiHeader = {};
    g_framebuffer.bitmap_info.bmiHeader.biSize = sizeof(g_framebuffer.bitmap_info.bmiHeader);
    g_framebuffer.bitmap_info.bmiHeader.biWidth = framebuffer.width;
    g_framebuffer.bitmap_info.bmiHeader.biHeight = -framebuffer.height; // TOP-down
    g_framebuffer.bitmap_info.bmiHeader.biPlanes = 1;
    g_framebuffer.bitmap_info.bmiHeader.biBitCount = APP_BPP;
    g_framebuffer.bitmap_info.bmiHeader.biCompression = BI_RGB;

    size_t memorySize = (size_t)(framebuffer.bytes_per_pixel * framebuffer.width * framebuffer.height);
    framebuffer.memory = VirtualAlloc(0, memorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

HWND Win32_CreateMainGameWindow(HINSTANCE instance)
{
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    windowClass.lpfnWndProc = &Win32_WindowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = LAPP_NAME;
    windowClass.hCursor = LoadCursor(0, IDC_ARROW);
    windowClass.hIcon = LoadIcon(0, IDI_APPLICATION);
    windowClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
    windowClass.hbrBackground = CreateSolidBrush(RGB(255, 0, 255));

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    if (!RegisterClassExW(&windowClass))
    {
        return NULL;
    }

    g_window_style = WS_OVERLAPPEDWINDOW;

    // TODO: Remove DWORD cast
    HWND window = CreateWindowW(
        windowClass.lpszClassName,
        windowClass.lpszClassName,
        (DWORD)g_window_style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        APP_RES_WIDTH,
        APP_RES_HEIGHT,
        0,
        0,
        instance,
        0
    );
    return window;
}

internal BOOL Win32_AppIsAlreadyRunning()
{
    HANDLE mutex = CreateMutexA(NULL, FALSE, APP_NAME "_MUTEX");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        return TRUE;
    }
    return FALSE;
}

internal void Win32_BlitDIBSection(HWND window)
{
    HDC context = GetDC(window);
    RECT clientRect;
    GetClientRect(window, &clientRect);
    i32 windowWidth = clientRect.right - clientRect.left;
    i32 windowHeight = clientRect.bottom - clientRect.top;
    StretchDIBits(
        context,
        0, 0, windowWidth, windowHeight,
        0, 0, g_framebuffer.buffer.width, g_framebuffer.buffer.height,
        g_framebuffer.buffer.memory,
        &g_framebuffer.bitmap_info,
        DIB_RGB_COLORS,
        SRCCOPY
    );

    ReleaseDC(window, context);
}

internal void Win32_ProcessPendingMessages(HWND window, Input& input)
{
    MSG message;
    Keyboard& keyboard = input.keyboard;
    Mouse& mouse = input.mouse;
    while (PeekMessage(&message, window,  0, 0, PM_REMOVE)) 
    {
        switch (message.message)
        {
            case WM_MOUSEWHEEL:
            {
                mouse.wheel_value = GET_WHEEL_DELTA_WPARAM(message.wParam);
            } break;

            case WM_MOUSEMOVE:
            {
                mouse.x_pos = GET_X_LPARAM(message.lParam);
                mouse.y_pos = GET_Y_LPARAM(message.lParam);

                // TODO: Move somewhere else? Cache it?
                RECT clientRect;
                GetClientRect(window, &clientRect);
                i32 width = clientRect.right - clientRect.left;
                i32 height = clientRect.bottom - clientRect.top;
                mouse.x_pos = (i32)((((f32)mouse.x_pos / (f32)width) * (f32)APP_RES_WIDTH));
                mouse.y_pos = (i32)((((f32)mouse.y_pos / (f32)height) * (f32)APP_RES_HEIGHT));
            } break;

            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_XBUTTONDOWN:
            {
                u32 mkCode = (u32)message.wParam;
                Win32ProcessMouseMessage(mouse, mkCode, true);
            } break;

            case WM_LBUTTONUP:
            {
                Win32ProcessMouseMessage(mouse, MK_LBUTTON, false);
            } break;
            case WM_MBUTTONUP:
            {
                Win32ProcessMouseMessage(mouse, MK_MBUTTON, false);
            } break;
            case WM_RBUTTONUP:
            {
                Win32ProcessMouseMessage(mouse, MK_RBUTTON, false);
            } break;
            case WM_XBUTTONUP:
            {
                // TODO: Might be a problem if both buttons are released at the same time
                u32 mkCode = (u32)message.wParam;

                if (mkCode | MK_XBUTTON1)
                {
                    Win32ProcessMouseMessage(mouse, MK_XBUTTON2, false);
                }
                else
                {
                    Win32ProcessMouseMessage(mouse, MK_XBUTTON1, false);
                }
            } break;

            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                u32 vk_code = (u32)message.wParam;
                u32 optCode = (u32)message.lParam;
                i32 wasDown = ((optCode & (1 << 30)) != 0) ? 1 : 0;
                i32 isDown = ((optCode & (1 << 31)) == 0) ? 1 : 0;
                i32 altKeyWasDown = ((optCode & (1 << 29)) != 0) ? 1 : 0;

                ButtonState& button = Win32GetKeyFromVkCode(keyboard, vk_code);
                button.isDown = (bool)isDown;

                if (keyboard.key_escape.isDown || ((keyboard.key_f4.isDown) && altKeyWasDown))
                {
                    g_running = false;
                }
                if (keyboard.key_f1.isDown)
                {
                    // https://stackoverflow.com/questions/2382464/win32-full-screen-and-hiding-taskbar
                    g_fullscreen = !g_fullscreen;
                    if (g_fullscreen)
                    {
                        GetWindowRect(window, &g_windowed_rect);
                        // TODO: EX_STYLE?
                        // TODO: Error handling
                        SetWindowLong(window, GWL_STYLE, g_window_style & ~(WS_CAPTION | WS_THICKFRAME));
                        /*SetWindowLong(window, GWL_EXSTYLE, g_window_ex_style & ~(WS_EX_DLGMODALFRAME |
                                    WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));*/
                        MONITORINFO monitor_info;
                        monitor_info.cbSize = sizeof(monitor_info);
                        GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor_info);
                        i32 monitor_width = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
                        i32 monitor_height = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;
                        SetWindowPos(window, HWND_TOP, monitor_info.rcMonitor.left, monitor_info.rcMonitor.top, monitor_width, monitor_height,
                                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
                    }
                    else
                    {
                        SetWindowLong(window, GWL_STYLE, g_window_style);
                        //SetWindowLong(window, GWL_EXSTYLE, g_window_ex_style);
                        i32 width = g_windowed_rect.right - g_windowed_rect.left;
                        i32 height = g_windowed_rect.bottom - g_windowed_rect.top;
                        SetWindowPos(window, HWND_TOP, g_windowed_rect.left, g_windowed_rect.top, width, height,
                                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
                    }
                }
                if (keyboard.key_f2.isDown)
                {
                    g_show_debug_info = !g_show_debug_info;
                }
            } break;

            default:
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            } break;
        }
    }
}

#include "win32/win32_input.cpp"
#include "win32/win32_opengl.cpp"
#include "app/app.cpp"