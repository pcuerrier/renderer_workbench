#pragma once

#pragma warning(disable : 4100) // unreferenced formal parameter
#pragma warning(disable : 4189) // local variable is initialized but not referenced
#pragma warning(disable : 4191) // unsafe cast
#pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable : 4505) // unreferenced local function has been removed
#pragma warning(disable : 4514) // 'function' : unreferenced inline function has been removed
#pragma warning(disable : 4820) // structure was padded due to alignment specifier
#pragma warning(disable : 5045) // Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified

#include <cstdint>

#define APP_RES_WIDTH	800
#define APP_RES_HEIGHT	600
#define APP_BPP		32
#define APP_DRAWING_AREA_MEMORY_SIZE	(APP_RES_WIDTH * APP_RES_HEIGHT * (APP_BPP / 8))

#define CALCULATE_PERF_TIME_EVERY_X_FRAMES	120
#define TARGET_MICROSECONDS_PER_FRAME		16667ULL
constexpr float TARGET_SECONDS_PER_FRAME = (TARGET_MICROSECONDS_PER_FRAME / 1000000.0f);

#define global   static
#define local    static
#define internal static

#define u8  std::uint8_t
#define u16 std::uint16_t
#define u32 std::uint32_t
#define u64 std::uint64_t

#define i8  std::int8_t
#define i16 std::int16_t
#define i32 std::int32_t
#define i64 std::int64_t

#define f32 float
#define f64 double

#define FLT_MAX 3.40282e+38f

#if DEBUG
#define Assert(expression) if(!(expression)) {*(volatile i32 *)0 = 0;}
#else
#define Assert(expression)
#endif

#define Kilobytes(value) ((value) * 1024uLL)
#define Megabytes(value) (Kilobytes(value) * 1024uLL)
#define Gigabytes(value) (Megabytes(value) * 1024uLL)
#define Terabytes(value) (Gigabytes(value) * 1024uLL)

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

#define ZeroArray(count, ptr) ZeroSize(count * sizeof((ptr)[0]), ptr)

internal void ZeroSize(size_t size, void* ptr)
{
    u8* byte = (u8*)ptr;
    while (size--)
    {
        *byte++ = 0;
    }
}

struct Framebuffer
{
    void* memory;
    i32 width;
    i32 height;
    i32 pitch;
    i32 bytes_per_pixel;
};

struct ButtonState
{
    bool isDown;
};

ButtonState INVALID_BUTTON_STATE = {};

struct Keyboard
{
    union
    {
        ButtonState keys[118];
        struct
        {
            ButtonState key_space;
            ButtonState key_apostrophe;
            ButtonState key_comma;
            ButtonState key_minus;
            ButtonState key_period;
            ButtonState key_slash;

            ButtonState key_0;
            ButtonState key_1;
            ButtonState key_2;
            ButtonState key_3;
            ButtonState key_4;
            ButtonState key_5;
            ButtonState key_6;
            ButtonState key_7;
            ButtonState key_8;
            ButtonState key_9;

            ButtonState key_semicolon;
            ButtonState key_equal;

            ButtonState key_a;
            ButtonState key_b;
            ButtonState key_c;
            ButtonState key_d;
            ButtonState key_e;
            ButtonState key_f;
            ButtonState key_g;
            ButtonState key_h;
            ButtonState key_i;
            ButtonState key_j;
            ButtonState key_k;
            ButtonState key_l;
            ButtonState key_m;
            ButtonState key_n;
            ButtonState key_o;
            ButtonState key_p;
            ButtonState key_q;
            ButtonState key_r;
            ButtonState key_s;
            ButtonState key_t;
            ButtonState key_u;
            ButtonState key_v;
            ButtonState key_w;
            ButtonState key_x;
            ButtonState key_y;
            ButtonState key_z;

            ButtonState key_left_bracket;
            ButtonState key_backslash;
            ButtonState key_right_bracket;
            ButtonState key_grave_accent;

            ButtonState key_escape;
            ButtonState key_enter;
            ButtonState key_tab;
            ButtonState key_backspace;

            ButtonState key_insert;
            ButtonState key_delete;
            ButtonState key_pageup;
            ButtonState key_pagedown;
            ButtonState key_home;
            ButtonState key_end;

            ButtonState key_right;
            ButtonState key_left;
            ButtonState key_down;
            ButtonState key_up;

            ButtonState key_capslock;
            ButtonState key_scrolllock;
            ButtonState key_numlock;
            ButtonState key_printscreen;
            ButtonState key_pause;

            ButtonState key_f1;
            ButtonState key_f2;
            ButtonState key_f3;
            ButtonState key_f4;
            ButtonState key_f5;
            ButtonState key_f6;
            ButtonState key_f7;
            ButtonState key_f8;
            ButtonState key_f9;
            ButtonState key_f10;
            ButtonState key_f11;
            ButtonState key_f12;
            ButtonState key_f13;
            ButtonState key_f14;
            ButtonState key_f15;
            ButtonState key_f16;
            ButtonState key_f17;
            ButtonState key_f18;
            ButtonState key_f19;
            ButtonState key_f20;
            ButtonState key_f21;
            ButtonState key_f22;
            ButtonState key_f23;
            ButtonState key_f24;
            ButtonState key_f25;

            ButtonState key_kp_0;
            ButtonState key_kp_1;
            ButtonState key_kp_2;
            ButtonState key_kp_3;
            ButtonState key_kp_4;
            ButtonState key_kp_5;
            ButtonState key_kp_6;
            ButtonState key_kp_7;
            ButtonState key_kp_8;
            ButtonState key_kp_9;

            ButtonState key_kp_period;
            ButtonState key_kp_divide;
            ButtonState key_kp_multiply;
            ButtonState key_kp_minus;
            ButtonState key_kp_plus;
            ButtonState key_kp_enter;
            ButtonState key_kp_equal;

            ButtonState key_left_shift;
            ButtonState key_left_control;
            ButtonState key_left_alt;
            ButtonState key_left_super;

            ButtonState key_right_shift;
            ButtonState key_right_control;
            ButtonState key_right_alt;
            ButtonState key_right_super;

            ButtonState key_menu;

            // NOTE: All buttonstate must be added above
            ButtonState terminator;
        };
    };
};

struct Mouse
{
    i32 wheel_value;
    i32 x_pos;
    i32 y_pos;

    ButtonState left;
    ButtonState right;
    ButtonState middle;
    ButtonState x1;
    ButtonState x2;
};

struct Input
{
    Keyboard keyboard;
    Mouse mouse;
};

struct Memory
{
    u64 permanent_storageSize;
    u64 transient_storageSize;
    void* permanent_storage;
    void* transient_storage;

    bool initialized = false;
};
