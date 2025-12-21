#include "win32_input.h"

internal ButtonState& Win32GetMouseKeyFromVkCode(Mouse& new_state, u32 mkCode);

/***********************************************************************************************************************
** Win32LoadXInput
***********************************************************************************************************************/
internal void Win32LoadXInput()
{
    HMODULE library = LoadLibraryA("xinput1_4.dll");
    if (!library)
    {
        library = LoadLibraryA("xinput9_1_0.dll");
    }

    if (!library)
    {
        library = LoadLibraryA("xinput1_3.dll");
    }

    if (library)
    {
        XInputGetState = (x_input_get_state*)((void*)GetProcAddress(library, "XInputGetState"));
        XInputSetState = (x_input_set_state*)((void*)GetProcAddress(library, "XInputSetState"));
    }
}

/***********************************************************************************************************************
** Win32ProcessKeyboardMessage
***********************************************************************************************************************/
/*internal void Win32ProcessKeyboardMessage(Keyboard& new_state, u32 vk_code, i32 is_down)
{
    ButtonState& button = Win32GetKeyFromVkCode(new_state, vk_code);
    if (button.is_down != is_down)
    {
        button.is_down = is_down;
        ++button.transitionCount;
    }
}*/

/***********************************************************************************************************************
** Win32ProcessMouseMessage
***********************************************************************************************************************/
internal void Win32ProcessMouseMessage(Mouse& new_state, u32 mkCode, i32 is_down)
{
    ButtonState& button = Win32GetMouseKeyFromVkCode(new_state, mkCode);
    if (button.is_down != (bool)is_down)
    {
        button.is_down = (bool)is_down;
    }
}

/***********************************************************************************************************************
** Win32ProcessXInputDigitalButton
***********************************************************************************************************************/
/*internal void Win32ProcessXInputDigitalButton(DWORD xInputButtonState,
                                              DWORD buttonBit,
                                              ButtonState* oldState,
                                              ButtonState* new_state)
{
    new_state->is_down = ((xInputButtonState & buttonBit) == buttonBit) ? 1 : 0;
    new_state->transitionCount = (oldState->is_down != new_state->is_down) ? 1 : 0;
}*/

/***********************************************************************************************************************
** Win32ProcessXInputStickValue
***********************************************************************************************************************/
/*internal f32 Win32ProcessXInputStickValue(SHORT value, SHORT deadZoneThreshold)
{
    f32 result = 0;

    if (value < -deadZoneThreshold)
    {
        result = (f32)((value + deadZoneThreshold) / (32768.0f - deadZoneThreshold));
    }
    else if (value > deadZoneThreshold)
    {
        result = (f32)((value - deadZoneThreshold) / (32767.0f - deadZoneThreshold));
    }

    return result;
}*/

/***********************************************************************************************************************
** Win32GetKeyFromVkCode
***********************************************************************************************************************/
internal ButtonState& Win32GetMouseKeyFromVkCode(Mouse& new_state, u32 mkCode)
{
    switch (mkCode)
    {
        case MK_LBUTTON:
        {
            return new_state.left;
        } break;

        case MK_MBUTTON:
        {
            return new_state.middle;
        } break;

        case MK_RBUTTON:
        {
            return new_state.right;
        } break;

        case MK_XBUTTON1:
        {
            return new_state.x1;
        } break;

        case MK_XBUTTON2:
        {
            return new_state.x2;
        } break;
    }
    return INVALID_BUTTON_STATE;
}

/***********************************************************************************************************************
** Win32GetKeyFromVkCode
***********************************************************************************************************************/
internal ButtonState& Win32GetKeyFromVkCode(Keyboard& new_state, u32 vk_code)
{
    switch (vk_code)
    {
        case VK_SPACE:
        {
            return new_state.key_space;
        } break;

        case VK_OEM_7:
        {
            return new_state.key_apostrophe;
        } break;

        case VK_OEM_COMMA:
        {
            return new_state.key_comma;
        } break;

        case VK_OEM_MINUS:
        {
            return new_state.key_minus;
        } break;

        case VK_OEM_PERIOD:
        {
            return new_state.key_period;
        } break;

        case VK_OEM_2:
        {
            return new_state.key_slash;
        } break;

        case '0':
        {
            return new_state.key_0;
        } break;

        case '1':
        {
            return new_state.key_1;
        } break;

        case '2':
        {
            return new_state.key_2;
        } break;

        case '3':
        {
            return new_state.key_3;
        } break;

        case '4':
        {
            return new_state.key_4;
        } break;

        case '5':
        {
            return new_state.key_5;
        } break;

        case '6':
        {
            return new_state.key_6;
        } break;

        case '7':
        {
            return new_state.key_7;
        } break;

        case '8':
        {
            return new_state.key_8;
        } break;

        case '9':
        {
            return new_state.key_9;
        } break;

        case VK_OEM_1:
        {
            return new_state.key_semicolon;
        } break;

        case VK_OEM_PLUS:
        {
            return new_state.key_equal;
        } break;

        case 'A':
        {
            return new_state.key_a;
        } break;

        case 'B':
        {
            return new_state.key_b;
        } break;

        case 'C':
        {
            return new_state.key_c;
        } break;

        case 'D':
        {
            return new_state.key_d;
        } break;

        case 'E':
        {
            return new_state.key_e;
        } break;

        case 'F':
        {
            return new_state.key_f;
        } break;

        case 'G':
        {
            return new_state.key_g;
        } break;

        case 'H':
        {
            return new_state.key_h;
        } break;

        case 'I':
        {
            return new_state.key_i;
        } break;

        case 'J':
        {
            return new_state.key_j;
        } break;

        case 'K':
        {
            return new_state.key_k;
        } break;

        case 'L':
        {
            return new_state.key_l;
        } break;

        case 'M':
        {
            return new_state.key_m;
        } break;

        case 'N':
        {
            return new_state.key_n;
        } break;

        case 'O':
        {
            return new_state.key_o;
        } break;

        case 'P':
        {
            return new_state.key_p;
        } break;

        case 'Q':
        {
            return new_state.key_q;
        } break;

        case 'R':
        {
            return new_state.key_r;
        } break;

        case 'S':
        {
            return new_state.key_s;
        } break;

        case 'T':
        {
            return new_state.key_t;
        } break;

        case 'U':
        {
            return new_state.key_u;
        } break;

        case 'V':
        {
            return new_state.key_v;
        } break;

        case 'W':
        {
            return new_state.key_w;
        } break;

        case 'X':
        {
            return new_state.key_x;
        } break;

        case 'Y':
        {
            return new_state.key_y;
        } break;

        case 'Z':
        {
            return new_state.key_z;
        } break;

        case VK_OEM_4:
        {
            return new_state.key_left_bracket;
        } break;

        case VK_OEM_5:
        {
            return new_state.key_backslash;
        } break;

        case VK_OEM_6:
        {
            return new_state.key_right_bracket;
        } break;

        case VK_OEM_3:
        {
            return new_state.key_grave_accent;
        } break;

        case VK_ESCAPE:
        {
            return new_state.key_escape;
        } break;

        case VK_RETURN:
        {
            return new_state.key_enter;
        } break;

        case VK_TAB:
        {
            return new_state.key_tab;
        } break;

        case VK_BACK:
        {
            return new_state.key_backspace;
        } break;

        case VK_INSERT:
        {
            return new_state.key_insert;
        } break;

        case VK_DELETE:
        {
            return new_state.key_delete;
        } break;

        case VK_PRIOR:
        {
            return new_state.key_pageup;
        } break;

        case VK_NEXT:
        {
            return new_state.key_pagedown;
        } break;

        case VK_HOME:
        {
            return new_state.key_home;
        } break;

        case VK_END:
        {
            return new_state.key_end;
        } break;

        case VK_RIGHT:
        {
            return new_state.key_right;
        } break;

        case VK_LEFT:
        {
            return new_state.key_left;
        } break;

        case VK_DOWN:
        {
            return new_state.key_down;
        } break;

        case VK_UP:
        {
            return new_state.key_up;
        } break;

        case VK_CAPITAL:
        {
            return new_state.key_capslock;
        } break;

        case VK_SCROLL:
        {
            return new_state.key_scrolllock;
        } break;

        case VK_NUMLOCK:
        {
            return new_state.key_numlock;
        } break;

        case VK_SNAPSHOT:
        {
            return new_state.key_printscreen;
        } break;

        case VK_PAUSE:
        {
            return new_state.key_pause;
        } break;

        case VK_F1:
        {
            return new_state.key_f1;
        } break;

        case VK_F2:
        {
            return new_state.key_f2;
        } break;

        case VK_F3:
        {
            return new_state.key_f3;
        } break;

        case VK_F4:
        {
            return new_state.key_f4;
        } break;

        case VK_F5:
        {
            return new_state.key_f5;
        } break;

        case VK_F6:
        {
            return new_state.key_f6;
        } break;

        case VK_F7:
        {
            return new_state.key_f7;
        } break;

        case VK_F8:
        {
            return new_state.key_f8;
        } break;

        case VK_F9:
        {
            return new_state.key_f9;
        } break;

        case VK_F10:
        {
            return new_state.key_f10;
        } break;

        case VK_F11:
        {
            return new_state.key_f11;
        } break;

        case VK_F12:
        {
            return new_state.key_f12;
        } break;

        case VK_F13:
        {
            return new_state.key_f13;
        } break;

        case VK_F14:
        {
            return new_state.key_f14;
        } break;

        case VK_F15:
        {
            return new_state.key_f15;
        } break;

        case VK_F16:
        {
            return new_state.key_f16;
        } break;

        case VK_F17:
        {
            return new_state.key_f17;
        } break;

        case VK_F18:
        {
            return new_state.key_f18;
        } break;

        case VK_F19:
        {
            return new_state.key_f19;
        } break;

        case VK_F20:
        {
            return new_state.key_f20;
        } break;

        case VK_F21:
        {
            return new_state.key_f21;
        } break;

        case VK_F22:
        {
            return new_state.key_f22;
        } break;

        case VK_F23:
        {
            return new_state.key_f23;
        } break;

        case VK_F24:
        {
            return new_state.key_f24;
        } break;

        case VK_NUMPAD0:
        {
            return new_state.key_kp_0;
        } break;

        case VK_NUMPAD1:
        {
            return new_state.key_kp_1;
        } break;

        case VK_NUMPAD2:
        {
            return new_state.key_kp_2;
        } break;

        case VK_NUMPAD3:
        {
            return new_state.key_kp_3;
        } break;

        case VK_NUMPAD4:
        {
            return new_state.key_kp_4;
        } break;

        case VK_CLEAR: // FALLTHROUGH
        case VK_NUMPAD5:
        {
            return new_state.key_kp_5;
        } break;

        case VK_NUMPAD6:
        {
            return new_state.key_kp_6;
        } break;

        case VK_NUMPAD7:
        {
            return new_state.key_kp_7;
        } break;

        case VK_NUMPAD8:
        {
            return new_state.key_kp_8;
        } break;

        case VK_NUMPAD9:
        {
            return new_state.key_kp_9;
        } break;

        case VK_DECIMAL:
        {
            return new_state.key_kp_period;
        } break;

        case VK_DIVIDE:
        {
            return new_state.key_kp_divide;
        } break;

        case VK_MULTIPLY:
        {
            return new_state.key_kp_multiply;
        } break;

        case VK_SUBTRACT:
        {
            return new_state.key_kp_minus;
        } break;

        case VK_ADD:
        {
            return new_state.key_kp_plus;
        } break;

        /*case SAME_AS_RETURN:
        {
            return new_state.key_kp_enter;
        } break;*/

        case VK_OEM_NEC_EQUAL:
        {
            return new_state.key_kp_equal;
        } break;

        case VK_LSHIFT:
        {
            return new_state.key_left_shift;
        } break;

        case VK_LCONTROL:
        {
            return new_state.key_left_control;
        } break;

        case VK_LMENU:
        {
            return new_state.key_left_alt;
        } break;

        case VK_LWIN:
        {
            return new_state.key_left_super;
        } break;

        case VK_RSHIFT:
        {
            return new_state.key_right_shift;
        } break;

        case VK_RCONTROL:
        {
            return new_state.key_right_control;
        } break;

        case VK_RMENU:
        {
            return new_state.key_right_alt;
        } break;

        case VK_RWIN:
        {
            return new_state.key_right_super;
        } break;

        case VK_MENU:
        {
            return new_state.key_menu;
        } break;
    }

    return INVALID_BUTTON_STATE;
}
