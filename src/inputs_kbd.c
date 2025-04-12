#include <dinput.h>
#include <sys\timeb.h>
#include "nespy.h"
#include "inputs.h"
#include <stdint.h>

LPDIRECTINPUT8 kbd_di;
LPDIRECTINPUTDEVICE8 keyboard;
uint8_t keyboardstate[0x100];
int kbd_up, kbd_down, kbd_left, kbd_right, kbd_a, kbd_b, kbd_start, kbd_select;
int kbd_snes_l, kbd_snes_r, kbd_snes_a, kbd_snes_x;
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
        int result = 0;
        result = result | (keyboardstate[kbd_a] > 0 ? 0b000000000001 : 0);
        result = result | (keyboardstate[kbd_b] > 0 ? 0b000000000010 : 0);
        result = result | (keyboardstate[kbd_start] > 0 ? 0b000000000100 : 0);
        result = result | (keyboardstate[kbd_select] > 0 ? 0b000000001000 : 0);
        result = result | (keyboardstate[kbd_up] > 0 ? 0b000000010000 : 0);
        result = result | (keyboardstate[kbd_down] > 0 ? 0b000000100000 : 0);
        result = result | (keyboardstate[kbd_left] > 0 ? 0b000001000000 : 0);
        result = result | (keyboardstate[kbd_right] > 0 ? 0b000010000000 : 0);
        result = result | (keyboardstate[kbd_snes_a] > 0 ? 0b000100000000 : 0);
        result = result | (keyboardstate[kbd_snes_x] > 0 ? 0b001000000000 : 0);
        result = result | (keyboardstate[kbd_snes_r] > 0 ? 0b010000000000 : 0);
        result = result | (keyboardstate[kbd_snes_l] > 0 ? 0b100000000000 : 0);
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
