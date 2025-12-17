#pragma once

#pragma warning(push, 3)
#include <Windows.h>
#include <Windowsx.h>
#include <Psapi.h>

#include <Xinput.h>
#include <stdio.h>
#pragma warning(pop)

#include "core.h"

#define WIN32_STATE_FILE_NAME_COUNT MAX_PATH

typedef LONG(NTAPI* _NtQueryTimerResolution) (OUT PULONG MinimumResolution, OUT PULONG MaximumResolution, OUT PULONG CurrentResolution);
_NtQueryTimerResolution NtQueryTimerResolution;

template<typename type>
struct Win32Stat
{
    type avg;
    type min;
    type max;
};

struct Win32AppPerfData
{
	u64 total_frame_rendered;	

	Win32Stat<f32> fps_raw;
	Win32Stat<f32> fps_cooked;

    Win32Stat<f32> ms_raw;
    Win32Stat<f32> ms_cooked;

    Win32Stat<u64> cycles_raw;
    Win32Stat<u64> cycles_cooked;

	i64 perf_frenquency;	

	//MONITORINFO MonitorInfo;
	//int32_t MonitorWidth;
	//int32_t MonitorHeight;
	ULONG minimum_timer_resolution;
	ULONG maximum_timer_resolution;
	ULONG current_timer_resolution;

    DWORD handle_count;
    PROCESS_MEMORY_COUNTERS_EX mem_info;

    SYSTEM_INFO system_info;
    i64 current_system_time;
	i64 previous_system_time;
	FILETIME process_creation_time;
	FILETIME process_exit_time;
	i64 current_user_cpu_time;
	i64 current_kernel_cpu_time;
	i64 previous_user_cpu_time;
	i64 previous_kernel_cpu_time;
	f64 cpu_percent;
};

struct Win32WindowDimensions
{
    i32 width;
    i32 height;
};

struct Win32Framebuffer
{
    BITMAPINFO bitmap_info;
    Framebuffer buffer;
};
