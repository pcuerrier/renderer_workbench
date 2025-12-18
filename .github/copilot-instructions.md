# Copilot Instructions for renderer_workbench

## Project Overview
A learning playground for 3D renderer development. The project architecture is designed to support incremental exploration of graphics APIs: starting with OpenGL for foundational concepts, then advancing to Vulkan for low-level control, and finally DirectX for platform-native performance. Currently uses Win32 for cross-API window/input management and a custom frame-buffer for baseline rendering.

## Architecture & Components

### Core Architecture
- **Platform Layer** (`src/win32/`): Windows-specific window management, input processing, DIB blitting, and performance monitoring. Decoupled via `core.h` types.
- **Core Types** (`src/core.h`): Shared definitions for memory, framebuffer, input structures, and type aliases (`u32`, `f32`, etc.).
- **Utilities** (`src/utils/`): Math functions (rounding, min/max) from `handmade_math.h`.

### Key Data Structures
- `Framebuffer`: Holds pixel buffer pointer, width, height, pitch, bytes-per-pixel.
- `Input`: Contains `Keyboard` (118 keys as union) and `Mouse` (position, wheel, 5 buttons).
- `Memory`: Permanent and transient storage pools (application memory model). To transitioned to memory arenas.
- `Win32AppPerfData`: FPS, milliseconds, CPU cycles stats (raw and cooked averages).

### Entry Point & Main Loop
- **`win32_main.cpp::WinMain()`**: Sets up timer resolution, process/thread priority, loads ntdll.dll for timer functions.
- **Message loop**: `Win32_ProcessPendingMessages()` feeds `Input` struct; main loop manages frame timing targeting 60 FPS (16,667 µs/frame).
- **Performance tracking**: Collects metrics every 120 frames; includes memory info (PSapi.h).

## Build System

### Command: `build.bat`
Runs the MSVC compiler with custom flags:
- **Includes**: `-I..\src\` (relative include path).
- **Key Flags**: `-Od` (no optimization), `-Z7` (debug info), `-WX` (warnings as errors), `-Zo` (optimized debug), `-GS-` (no stack checks), `-Gs9999999` (no stack probes).
- **Linker**: Windows subsystem, libs: `user32.lib`, `gdi32.lib`, `winmm.lib`, `kernel32.lib`.
- **Output**: `build/win32.exe` + `win32.map`.
- **Workflow**: Delete old PDB, compile, link—no rebuild detection; run `build.bat` each change.

## Coding Conventions & Patterns

### Type System
- Explicitly use fixed-size types: `u32`, `i32`, `f32`, etc. (mapped to std::uint32_t, etc.).
- Avoid `int`, `float`; prefer the type macros.

### Scoping
- `global` = file-scoped static (shared across functions in translation unit).
- `local` = block-scoped static (persists across calls).
- `internal` = function-scoped static or unnamed namespace equivalent.
- Example: `global bool g_running;` signals module-level state.

### Macro Utilities
- **Memory**: `Kilobytes(x)`, `Megabytes(x)` for size calculations.
- **Arrays**: `ArrayCount(array)` for compile-time array sizes.
- **Debugging**: `Assert(expr)` writes to NULL in DEBUG mode (crash breakpoint).

### Win32 Patterns
- **Dynamic Loading**: XInput functions load dynamically; stubs return `ERROR_DEVICE_NOT_CONNECTED` if unavailable (see `win32_input.h`).
- **DIB Rendering**: `Win32_ResizeDIBSection()` and `Win32_BlitDIBSection()` manage pixel buffer; hardcoded to 800×450 resolution.
- **Window Proc**: Simple case-switch; message handling defers to `DefWindowProc()`.

## Code Syntax Standard

### Naming Conventions
- **Global variables**: Prefix with `g_` (e.g., `g_running`, `g_framebuffer`, `g_perf_data`).
- **Win32-specific functions**: Prefix with `Win32_` (e.g., `Win32_ProcessPendingMessages()`, `Win32_BlitDIBSection()`).
- **Functions**: Use `PascalCase` for function names (e.g., `WinMain()`, `Win32_CreateMainGameWindow()`).
- **Structs and types**: Use `PascalCase` (e.g., `Framebuffer`, `Input`, `Win32AppPerfData`).
- **Constants/macros**: Use `UPPER_SNAKE_CASE` (e.g., `APP_RES_WIDTH`, `TARGET_MICROSECONDS_PER_FRAME`).
- **Local variables and parameters**: Use `snake_case` (e.g., `input`, `button_state`, `memory_size`).

### Code Style
- **Indentation**: 4 spaces (consistent with existing code).
- **Brackets**: Opening bracket on new line (Allman style), closing bracket on new line for blocks.
- **Line length**: No strict limit; prioritize readability over fixed column width.
- **Comments**: Use `//` for single-line and block comments; explain "why" not "what".
- **Includes**: Platform-specific headers first (`Windows.h`, `Xinput.h`), then project headers (`core.h`), then standard library.

### Type and Variable Usage
- Always initialize structs and arrays when declaring them; prefer explicit initialization over relying on defaults.
- Use `{}` initialization for zero-initialization (e.g., `ButtonState button = {}`).
- Prefer stack allocation for fixed-size objects; use custom memory pools (`Memory` struct) for dynamic allocation.
- Avoid pointer arithmetic where possible; use struct fields or explicit indexing.

### Function Signatures
- Use `internal` for static functions local to a file; use `internal void` for functions with side effects.
- Return types explicit; avoid implicit conversions (cast if needed).
- Parameter order: inputs first, then outputs (struct references); use `const` sparingly (project doesn't emphasize it).
- Use `&` for output parameters (e.g., `Input& input`); avoid raw pointers except for memory pools.

## Critical Developer Workflows

### Running the Build
```bash
# From workspace root
build.bat
# Output: build/win32.exe, build/win32.map, build/*.pdb
```
In VS Code, use the build task: **Run task → Build Debug**.

### Debugging
- PDB files generated in `build/` directory; attach debugger to `win32.exe`.
- `Assert()` macro triggers NULL dereference breakpoint in DEBUG mode.
- Check `g_perf_data` struct for runtime metrics (FPS, frame time, CPU cycles, memory).

### Adding Features
1. **Input**: Extend `Keyboard` or `Mouse` in `core.h` struct; handle in `Win32_ProcessKeyboardMessage()` or `Win32_ProcessMouseMessage()`.
2. **Rendering**: Draw to `g_framebuffer.memory` (32-bit BGRA pixels); respect 800×450 resolution.
3. **Application Logic**: Implement in main loop; access input via `Input` struct passed to `Win32_ProcessPendingMessages()`.

## Integration Points & Dependencies

- **Windows API**: `Windows.h`, `Windowsx.h`, `Psapi.h` for process/memory info.
- **XInput**: Dynamically loaded via `winmm.lib`; controller support optional.
- **DIB Rendering**: GDI32 (`gdi32.lib`) for `SetDIBitsToDevice()` blitting.
- **Timer Functions**: ntdll.dll `NtQueryTimerResolution()` for precision timing.
- **No external graphics libs**: All pixel manipulation is manual; useful for learning fundamentals.

## Notes for Agents
- **Compiler warnings disabled**: See `core.h` pragma disables; meaningful warnings still treated as errors (`-WX`).
- **Fixed resolution**: 800×450 @ 32 bpp hardcoded; changing requires edits to `APP_RES_*` macros.
- **Frame timing**: Targets 60 FPS via `TARGET_SECONDS_PER_FRAME` constant; actual sleep-based frame rate control in main loop.
- **Performance instrumentation**: Built-in FPS/cycle tracking; no external profiler needed for basic metrics.
