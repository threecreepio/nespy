#include <dinput.h>
#include <sys\timeb.h>
#include "nespy.h"
#include "inputs.h"
#include <stdint.h>

LPDIRECTINPUT8 kbd_di;
LPDIRECTINPUTDEVICE8 keyboard;
uint8_t keyboardstate[0x100];
int kbd_up = -1, kbd_down = -1, kbd_left = -1, kbd_right = -1, kbd_a = -1, kbd_b = -1, kbd_start = -1, kbd_select = -1;
int kbd_snes_l = -1, kbd_snes_r = -1, kbd_snes_a = -1, kbd_snes_x = -1;
int kbd_debug = 0;
int kbd_socd = SOCD_NEUTRAL;

int KBDConfigure()
{
    HRESULT hr;
    if (FAILED(hr = IDirectInputDevice8_SetDataFormat(keyboard, &c_dfDIKeyboard))) {
        return hr;
    }
    if (FAILED(hr = IDirectInputDevice8_SetCooperativeLevel(keyboard, NULL, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND))) {
        return hr;
    }
    return 0;
}

int KBDPoll(void)
{
    HRESULT hr;
    hr = IDirectInputDevice8_Poll(keyboard);
    if (FAILED(hr)) {
        while (1) {
            hr = IDirectInputDevice8_Acquire(keyboard);
            if (hr == DIERR_INPUTLOST) {
                printf("input lost: %ld\n", hr);
                continue;
            }
            if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED)) {
                printf("failed to access keyboard: %ld\n", hr);
                return -1;
            }
            if (FAILED(hr)) {
                printf("couldnt acquire keyboard: %ld\n", hr);
                return -1;
            }
            break;
        }
        hr = IDirectInputDevice8_Poll(keyboard);
        if (FAILED(hr)) {
            printf("couldnt poll keyboard: %ld\n", hr);
            return -1;
        }
    }
    IDirectInputDevice8_GetDeviceState(keyboard, 0x100, &keyboardstate);
    return 0;
}

DWORD WINAPI KBDThread(void* data)
{
    HANDLE updateEvent = CreateEvent(NULL, FALSE, FALSE, "Update");
    if (updateEvent == NULL) {
        printf("failed to create update event.\n");
        exit(-2);
    }
    IDirectInputDevice8_SetEventNotification(keyboard, updateEvent);
    while (1) {
        if (KBDPoll() != 0) {
            exit(-2);
        }
        if (kbd_debug) {
            for (int i = 0; i < 0x100; ++i) {
                if (keyboardstate[i]) {
                    fprintf(stdout, "pressed input: $%02X\n", i);
                }
            }
        }
        #define INP(bit, name) (name > -1 && keyboardstate[name]) ? bit : 0;
        int result = 0;
        result |= INP(0x001, kbd_a);
        result |= INP(0x002, kbd_b);
        result |= INP(0x004, kbd_start);
        result |= INP(0x008, kbd_select);
        result |= INP(0x010, kbd_up);
        result |= INP(0x020, kbd_down);
        result |= INP(0x040, kbd_left);
        result |= INP(0x080, kbd_right);
        result |= INP(0x100, kbd_snes_a);
        result |= INP(0x200, kbd_snes_x);
        result |= INP(0x400, kbd_snes_r);
        result |= INP(0x800, kbd_snes_l);
        result = handleSOCD(result);
        updateInputState(result, 1);
    }
}

BOOL CALLBACK KBDEnumDevices(const DIDEVICEINSTANCE* did, void* ctx)
{
    fprintf(logfile, "using device %s\n", did->tszInstanceName);
    HRESULT result = IDirectInput8_CreateDevice(kbd_di, &did->guidInstance, &keyboard, NULL);
    if (FAILED(result)) {
        fprintf(logfile, "device failed %s\n", did->tszInstanceName);
        return DIENUM_CONTINUE;
    }
    return DIENUM_STOP;
}

int KBDInit()
{
    inputErrorCode = 1;
    keyboard = 0;
    if (FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, &IID_IDirectInput8, (void**)&kbd_di, NULL))) {
        return -1;
    }
    if (FAILED(IDirectInput8_EnumDevices(kbd_di, DI8DEVCLASS_KEYBOARD, KBDEnumDevices, GetModuleHandle(NULL), DIEDFL_ATTACHEDONLY))) {
        return -2;
    }
    if (0 == keyboard) {
        fprintf(logfile, "failed to find a keyboard\n");
        return -3;
    }
    KBDConfigure();
    KBDPoll();
    HANDLE thread = CreateThread(NULL, 0, KBDThread, NULL, 0, NULL);
    if (!thread) {
        fprintf(logfile, "could not spawn thread\n");
        return -4;
    }

    inputErrorCode = 0;
    return 0;
}

int KBDInputReadSetting(void* user, const char* section, const char* name, const char* value)
{
    if (SETTING("KEYBOARD", "debug")) kbd_debug = strtol(value, NULL, 10);
    if (SETTING("KEYBOARD", "up")) kbd_up = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "down")) kbd_down = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "left")) kbd_left = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "right")) kbd_right = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "a")) kbd_a = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "b")) kbd_b = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "start")) kbd_start = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "select")) kbd_select = keynameToKeyCode(value);

    if (snesmode) {
        if (SETTING("KEYBOARD", "y")) kbd_a = keynameToKeyCode(value);
        if (SETTING("KEYBOARD", "a")) kbd_snes_a = keynameToKeyCode(value);
        if (SETTING("KEYBOARD", "l")) kbd_snes_l = keynameToKeyCode(value);
        if (SETTING("KEYBOARD", "r")) kbd_snes_r = keynameToKeyCode(value);
        if (SETTING("KEYBOARD", "x")) kbd_snes_x = keynameToKeyCode(value);
    }

    return 0;
}
