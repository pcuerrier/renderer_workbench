#ifndef WIN32_INPUT_H
#define WIN32_INPUT_H

#include "win32_main.h"

internal void Win32LoadXInput();
internal ButtonState& Win32GetKeyFromVkCode(Keyboard& new_state, u32 vk_code);
internal void Win32ProcessKeyboardMessage(Keyboard& new_state, u32 vk_code, i32 is_down);
internal void Win32ProcessMouseMessage(Mouse& new_state, u32 mkCode, i32 is_down);
internal void Win32ProcessXInputDigitalButton(DWORD xInputButtonState, DWORD buttonBit, ButtonState* oldState, ButtonState* new_state);
internal f32 Win32ProcessXInputStickValue(SHORT value, SHORT deadZoneThreshold);

//**********************************************************************************************************************
// XInput Dynamic loading
//**********************************************************************************************************************

/***********************************************************************************************************************
** XInputGetState
***********************************************************************************************************************/
typedef DWORD WINAPI x_input_get_state(DWORD dwUserIndex, XINPUT_STATE *pState);
DWORD WINAPI XInputGetStateStub(DWORD /*dwUserIndex*/, XINPUT_STATE * /*pState*/)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
internal x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

/***********************************************************************************************************************
** XInputSetState
***********************************************************************************************************************/
typedef DWORD WINAPI x_input_set_state(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration);
DWORD WINAPI XInputSetStateStub(DWORD /*dwUserIndex*/, XINPUT_VIBRATION * /*pVibration*/)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
internal x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

#endif // WIN32_INPUT_H
