#include <dinput.h>
#include <sys\timeb.h>
#include "nespy.h"
#include "inputs.h"

char joy_device[256] = "";
float joy_framerate = 16.6393322f;
LPDIRECTINPUT8 di;
LPDIRECTINPUTDEVICE8 joypad;
DIJOYSTATE2 joypadstate;
DIDEVCAPS joypadcaps;
byte btnstate[32 + 4];
int joy_up, joy_down, joy_left, joy_right, joy_a, joy_b, joy_start, joy_select;
int joy_snes_l, joy_snes_r, joy_snes_a, joy_snes_x;

int JOYConfigure()
{
    HRESULT hr;
    if (FAILED(hr = IDirectInputDevice8_SetDataFormat(joypad, &c_dfDIJoystick2))) {
        return hr;
    }
    if (FAILED(hr = IDirectInputDevice8_SetCooperativeLevel(joypad, NULL, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND))) {
        return hr;
    }
    joypadcaps.dwSize = sizeof(DIDEVCAPS);
    joypadcaps.dwButtons = 8;
    if (FAILED(hr = IDirectInputDevice8_GetCapabilities(joypad, &joypadcaps))) {
        return hr;
    }
    
    return 0;
}

int JOYPoll(void)
{
    HRESULT hr;
    hr = IDirectInputDevice8_Poll(joypad);
    if (FAILED(hr)) {
        while (1) {
            hr = IDirectInputDevice8_Acquire(joypad);
            if (hr == DIERR_INPUTLOST) {
                printf("input lost... %ld\n", hr);
                continue;
            }
            if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED)) {
                printf("failed to access joypad: %ld\n", hr);
                return -1;
            }
            if (FAILED(hr)) {
                printf("couldnt acquire joypad: %ld\n", hr);
                return -1;
            }
            break;
        }
        hr = IDirectInputDevice8_Poll(joypad);
        if (FAILED(hr)) {
            printf("couldnt poll joypad: %ld\n", hr);
            return -1;
        }
    }
    IDirectInputDevice8_GetDeviceState(joypad, sizeof(DIJOYSTATE2), &joypadstate);
    return 0;
}

DWORD WINAPI JOYThread(void* data)
{
    HANDLE updateEvent = CreateEvent(NULL, FALSE, FALSE, "Update");
    if (updateEvent == NULL) {
        printf("failed to create update event.\n");
        exit(-2);
    }
    IDirectInputDevice8_SetEventNotification(joypad, updateEvent);
    byte buttons[32 + 4];
    while (1) {
        if (JOYPoll() != 0) {
            exit(-2);
        }
        int dpad = joypadstate.rgdwPOV[0];
        for (int i = 0; i < 32; ++i) buttons[i] = joypadstate.rgbButtons[i];
        buttons[32] = dpad != -1 && (dpad > 27000 || dpad < 9000);
        buttons[33] = dpad != -1 && (dpad < 18000 && dpad > 0);
        buttons[34] = dpad != -1 && (dpad < 27000 && dpad > 9000);
        buttons[35] = dpad != -1 && (dpad > 18000);

        int result = 0;
        result = result | (buttons[joy_a] > 0 ? 0b000000000001 : 0);
        result = result | (buttons[joy_b] > 0 ? 0b000000000010 : 0);
        result = result | (buttons[joy_start] > 0 ? 0b000000000100 : 0);
        result = result | (buttons[joy_select] > 0 ? 0b000000001000 : 0);
        result = result | (buttons[joy_up] > 0 ? 0b000000010000 : 0);
        result = result | (buttons[joy_down] > 0 ? 0b000000100000 : 0);
        result = result | (buttons[joy_left] > 0 ? 0b000001000000 : 0);
        result = result | (buttons[joy_right] > 0 ? 0b000010000000 : 0);
        result = result | (buttons[joy_snes_a] > 0 ? 0b000100000000 : 0);
        result = result | (buttons[joy_snes_x] > 0 ? 0b001000000000 : 0);
        result = result | (buttons[joy_snes_r] > 0 ? 0b010000000000 : 0);
        result = result | (buttons[joy_snes_l] > 0 ? 0b100000000000 : 0);
        result = handleSOCD(result);

        if (result == currentInputs) continue;
        updateInputState(result, joy_framerate, 1);
    }
}

BOOL CALLBACK JOYEnumJoypad(const DIDEVICEINSTANCE* did, void* ctx)
{
    if (!joy_device[0] || strcmp(joy_device, did->tszInstanceName) == 0) {
        fprintf(logfile, "using device %s\n", did->tszInstanceName);
        HRESULT result = IDirectInput8_CreateDevice(di, &did->guidInstance, &joypad, NULL);
        if (FAILED(result)) {
            fprintf(logfile, "device failed %s\n", did->tszInstanceName);
            return DIENUM_CONTINUE;
        }
        return DIENUM_STOP;
    }
    fprintf(logfile, "skipping device %s\n", did->tszInstanceName);
    return DIENUM_CONTINUE;
}

int JOYInit()
{
    inputErrorCode = 1;
    joypad = 0;
    if (FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, &IID_IDirectInput8, (void**)&di, NULL))) {
        return -1;
    }
    if (FAILED(IDirectInput8_EnumDevices(di, DI8DEVCLASS_GAMECTRL, JOYEnumJoypad, GetModuleHandle(NULL), DIEDFL_ATTACHEDONLY))) {
        return -2;
    }
    if (0 == joypad) {
        fprintf(logfile, "failed to find a joypad\n");
        return -3;
    }
    JOYConfigure();
    JOYPoll();
    HANDLE thread = CreateThread(NULL, 0, JOYThread, NULL, 0, NULL);
    if (!thread) {
        fprintf(logfile, "could not spawn thread\n");
        return -4;
    }
    
    inputErrorCode = 0;
    return 0;
}

int JOYInputReadSetting(void* user, const char* section, const char* name, const char* value)
{
    if (SETTING("JOYPAD", "fps")) joy_framerate = 1000.0f / strtof(value, NULL);
    if (SETTING("JOYPAD", "device")) snprintf(joy_device, sizeof(joy_device), "%s", value);
    if (SETTING("JOYPAD", "up")) joy_up = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "down")) joy_down = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "left")) joy_left = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "right")) joy_right = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "a")) joy_a = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "b")) joy_b = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "start")) joy_start = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "select")) joy_select = joynameToKeyCode(value);
    
    if (snesmode) {
        if (SETTING("JOYPAD", "y")) joy_a = joynameToKeyCode(value);
        if (SETTING("JOYPAD", "a")) joy_snes_a = joynameToKeyCode(value);
        if (SETTING("JOYPAD", "l")) joy_snes_l = joynameToKeyCode(value);
        if (SETTING("JOYPAD", "r")) joy_snes_r = joynameToKeyCode(value);
        if (SETTING("JOYPAD", "x")) joy_snes_x = joynameToKeyCode(value);
    }

    return 0;
}
