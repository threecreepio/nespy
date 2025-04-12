#include <dinput.h>
#include <sys\timeb.h>
#include "nespy.h"
#include "inputs.h"

char joy_device[256] = "";
LPDIRECTINPUT8 di;
LPDIRECTINPUTDEVICE8 joypad;
DIJOYSTATE2 joypadstate;
DIDEVCAPS joypadcaps;
byte btnstate[32 + 4];
int joy_up = -1, joy_down = -1, joy_left = -1, joy_right = -1, joy_a = -1, joy_b = -1, joy_start = -1, joy_select = -1;
int joy_snes_l = -1, joy_snes_r = -1, joy_snes_a = -1, joy_snes_x = -1;

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
    byte buttons[0x400];
    while (1) {
        if (JOYPoll() != 0) {
            exit(-2);
        }
        int dpad = joypadstate.rgdwPOV[0];
        for (int i = 0; i < 128; ++i) buttons[i] = joypadstate.rgbButtons[i];
        buttons[1020] = dpad != -1 && (dpad > 27000 || dpad < 9000);
        buttons[1021] = dpad != -1 && (dpad < 18000 && dpad > 0);
        buttons[1022] = dpad != -1 && (dpad < 27000 && dpad > 9000);
        buttons[1023] = dpad != -1 && (dpad > 18000);

        #define INP(bit, name) (name > -1 && buttons[name]) ? bit : 0;
        int result = 0;
        result |= INP(0x001, joy_a);
        result |= INP(0x002, joy_b);
        result |= INP(0x004, joy_start);
        result |= INP(0x008, joy_select);
        result |= INP(0x010, joy_up);
        result |= INP(0x020, joy_down);
        result |= INP(0x040, joy_left);
        result |= INP(0x080, joy_right);
        result |= INP(0x100, joy_snes_a);
        result |= INP(0x200, joy_snes_x);
        result |= INP(0x400, joy_snes_r);
        result |= INP(0x800, joy_snes_l);
        result = handleSOCD(result);
        updateInputState(result, 1);
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
