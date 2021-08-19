
#include <windows.h>
#include <tchar.h>
#include <sys\timeb.h>
#include <stdio.h>
#include <stdint.h>
#include "nespy.h"
#include "inputs.h"
#include "dinput.h"
int joynameToKeyCode(const char *input);

#define NESKEY_UP 0x10
#define NESKEY_DOWN 0x20
#define NESKEY_LEFT 0x40
#define NESKEY_RIGHT 0x80
#define NESKEY_A 0x01
#define NESKEY_B 0x02
#define NESKEY_S 0x04
#define NESKEY_T 0x08

#define KBDLR_NONE 0
#define KBDLR_BIZHAWK 1
#define KBDLR_FCEUX 2
#define KBDLR_MESEN 3

HANDLE nes_comport;
char inputtype[50] = "NES";
char comport[50] = "";
float framerate = 16.6393322f;
float joy_framerate = 16.6393322f;
int downKeys = 0;
char joy_device[256] = "";
int inputState = 0;
int repeatedInputs = 0;
struct timeb currentTimer;
int nes_suminputs = 1;
int nes_ignoreextrareads = 1;

void print_inputstate(int newInput) {
    printf("%s%s%s%s%s%s%s%s",
        (newInput & NESKEY_UP) == NESKEY_UP ? "U" : " ",
        (newInput & NESKEY_DOWN) == NESKEY_DOWN ? "D" : " ",
        (newInput & NESKEY_LEFT) == NESKEY_LEFT ? "L" : " ",
        (newInput & NESKEY_RIGHT) == NESKEY_RIGHT ? "R" : " ",
        (newInput & NESKEY_S) == NESKEY_S ? "S" : " ",
        (newInput & NESKEY_T) == NESKEY_T ? "T" : " ",
        (newInput & NESKEY_B) == NESKEY_B ? "B" : " ",
        (newInput & NESKEY_A) == NESKEY_A ? "A" : " "
    );
}

void logInputChangeNES(int nextByte) {
    struct timeb end;
    ftime(&end);
    float diff = (float) ((1000.0 * (end.time - currentTimer.time) + (end.millitm - currentTimer.millitm)) / framerate);
    if (nes_ignoreextrareads && nextByte == currentInputs && diff < 0.05f) return;
    if (currentInputs == nextByte) repeatedInputs += 1;
    if (nextByte != currentInputs && nes_suminputs) {
        printf(" : %d\n", repeatedInputs);
        repeatedInputs = 1;
    }
    if (nextByte != currentInputs || !nes_suminputs) {
        print_inputstate(nextByte);
        if (!nes_suminputs) printf("\n");
    }
    currentTimer = end;
}


LPDIRECTINPUT8 di;
LPDIRECTINPUTDEVICE8 joypad;
DIJOYSTATE2 joypadstate;
DIDEVCAPS joypadcaps;
byte btnstate[32 + 4];

int JOYConfigure() {
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

int JOYPoll(void) {
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

int joy_up, joy_down, joy_left, joy_right, joy_a, joy_b, joy_start, joy_select;
int joy_debug = 0;
DWORD WINAPI JOYThread(void* data) {
    HANDLE updateEvent = CreateEvent(NULL,FALSE,FALSE,"Update");
    if (updateEvent == NULL) {
        printf("failed to create event..\n");
        return -2;
    }
    IDirectInputDevice8_SetEventNotification(joypad, updateEvent);
    byte buttons[32 + 4];
    while (1) {
        if (JOYPoll() != 0) {
            exit(-2);
        }
        int dpad = joypadstate.rgdwPOV[0];
        for (int i=0; i<32; ++i) buttons[i] = joypadstate.rgbButtons[i];
        buttons[32] = dpad != -1 && (dpad > 27000 || dpad < 9000);
        buttons[33] = dpad != -1 && (dpad < 18000 && dpad > 0);
        buttons[34] = dpad != -1 && (dpad < 27000 && dpad > 9000);
        buttons[35] = dpad != -1 && (dpad > 18000);

        int result = 0;
        result = result | (buttons[joy_a]      > 0 ? 0b00000001 : 0);
        result = result | (buttons[joy_b]      > 0 ? 0b00000010 : 0);
        result = result | (buttons[joy_start]  > 0 ? 0b00000100 : 0);
        result = result | (buttons[joy_select] > 0 ? 0b00001000 : 0);
        result = result | (buttons[joy_up]     > 0 ? 0b00010000 : 0);
        result = result | (buttons[joy_down]   > 0 ? 0b00100000 : 0);
        result = result | (buttons[joy_left]   > 0 ? 0b01000000 : 0);
        result = result | (buttons[joy_right]  > 0 ? 0b10000000 : 0);
        if (result == currentInputs) continue;
        currentInputs = result;
        struct timeb end;
        ftime(&end);
        float diff = (float) ((1000.0 * (end.time - currentTimer.time) + (end.millitm - currentTimer.millitm)) / joy_framerate);
        printf(" : %3.2f\n", diff);
        print_inputstate(result);
        currentTimer = end;
    }
}

BOOL CALLBACK JOYEnumJoypad(const DIDEVICEINSTANCE *did, void *ctx) {
    if (strcmp(joy_device, did->tszInstanceName) == 0) {
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

int JOYInit() {
    joypad = 0;
    if (FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, &IID_IDirectInput8, (void**) &di, NULL))) {
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
    return 0;
}


int NESInit();
DWORD WINAPI NESThread(void* data) {
    uint8_t nextByte;
    DWORD readlen;
    while (1) {
            if (!ReadFile( nes_comport, &nextByte, sizeof(nextByte), &readlen, NULL)) {
                Sleep(2500);
                exit(-2);
            }
            if (!readlen) continue;
            logInputChangeNES(nextByte);
            currentInputs = nextByte;
    }
    return 0;
}

int NESInit() {
    nes_comport = CreateFile(comport, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (nes_comport == INVALID_HANDLE_VALUE) {
        fprintf(logfile, "could not connect to nes controller on %s\n", comport);
        return -1;
    }

    DCB param = { 0 };
    param.DCBlength = sizeof(param);
    int status = GetCommState(nes_comport, &param);
    if (status == 0) {
        unsigned long errn = GetLastError();
        fprintf(logfile, "could not read nes controller state on %s - status %ld\n", comport, errn);
        CloseHandle(comport);
        return -1;
    }

    param.BaudRate = CBR_115200;
    param.ByteSize = 8;
    param.StopBits = ONESTOPBIT;
    param.Parity   = NOPARITY;
    SetCommState(nes_comport, &param);
    status = SetCommMask(nes_comport, EV_RXCHAR);
    if (status == 0) {
        unsigned long errn = GetLastError();
        fprintf(logfile, "could not set nes controller state on %s - status %ld\n", comport, errn);
        CloseHandle(nes_comport);
        return -1;
    }

    HANDLE thread = CreateThread(NULL, 0, NESThread, NULL, 0, NULL);
    if (!thread) {
        fprintf(logfile, "could not spawn thread\n");
        CloseHandle(nes_comport);
        return -2;
    }

	return 0;
}

int kbd_lrmode, kbd_up, kbd_down, kbd_left, kbd_right, kbd_a, kbd_b, kbd_start, kbd_select;
int kbd_debug = 0;
LRESULT __stdcall KBDCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
    int newInput = inputState;
    
    if (wParam == WM_KEYDOWN) {
        if (kbdStruct.vkCode == kbd_up) {
            downKeys |= NESKEY_UP;
            int islr = downKeys & NESKEY_DOWN;
            if (kbd_lrmode == KBDLR_BIZHAWK) {
                newInput &= 0xFF ^ (NESKEY_UP | NESKEY_DOWN);
            }
            if (kbd_lrmode == KBDLR_MESEN && islr) {
                newInput &= 0xFF ^ NESKEY_DOWN;
            }
            newInput |= NESKEY_UP;
            if (kbd_lrmode == KBDLR_FCEUX && islr) {
                newInput &= 0xFF ^ (NESKEY_UP | NESKEY_DOWN);
            }
        }
        else if (kbdStruct.vkCode == kbd_down) {
            downKeys |= NESKEY_DOWN;
            int islr = downKeys & NESKEY_UP;
            if (kbd_lrmode == KBDLR_BIZHAWK) {
                newInput &= 0xFF ^ (NESKEY_UP | NESKEY_DOWN);
            }
            if (kbd_lrmode == KBDLR_MESEN && islr) {}
            else newInput |= NESKEY_DOWN;
            if (kbd_lrmode == KBDLR_FCEUX && islr) {
                newInput &= 0xFF ^ (NESKEY_UP | NESKEY_DOWN);
            }
        }
        else if (kbdStruct.vkCode == kbd_left) {
            downKeys |= NESKEY_LEFT;
            int islr = downKeys & NESKEY_RIGHT;
            if (kbd_lrmode == KBDLR_BIZHAWK && islr) {
                newInput &= 0xFF ^ (NESKEY_LEFT | NESKEY_RIGHT);
            }
            if (kbd_lrmode == KBDLR_MESEN && islr) {
                newInput &= 0xFF ^ NESKEY_RIGHT;
            }
            newInput |= NESKEY_LEFT;
            if (kbd_lrmode == KBDLR_FCEUX && islr) {
                newInput &= 0xFF ^ (NESKEY_LEFT | NESKEY_RIGHT);
            }
        }
        else if (kbdStruct.vkCode == kbd_right) {
            downKeys |= NESKEY_RIGHT;
            int islr = downKeys & NESKEY_LEFT;
            if (kbd_lrmode == KBDLR_BIZHAWK) {
                newInput &= 0xFF ^ (NESKEY_LEFT | NESKEY_RIGHT);
            }
            if (kbd_lrmode == KBDLR_MESEN && islr) {}
            else newInput |= NESKEY_RIGHT;
            if (kbd_lrmode == KBDLR_FCEUX && islr) {
                newInput &= 0xFF ^ (NESKEY_LEFT | NESKEY_RIGHT);
            }
        }
        else if (kbdStruct.vkCode == kbd_a) {
            downKeys |= NESKEY_A;
            newInput |= NESKEY_A;
        }
        else if (kbdStruct.vkCode == kbd_b) {
            downKeys |= NESKEY_B;
            newInput |= NESKEY_B;
        }
        else if (kbdStruct.vkCode == kbd_select) {
            downKeys |= NESKEY_S;
            newInput |= NESKEY_S;
        }
        else if (kbdStruct.vkCode == kbd_start) {
            downKeys |= NESKEY_T;
            newInput |= NESKEY_T;
        } else {
            return CallNextHookEx(0, nCode, wParam, lParam);
        }
    }

    
    if (wParam == WM_KEYUP) {
        if (kbd_debug) {
            printf("Key: %x\n", (int) kbdStruct.vkCode);
        }
        if (kbdStruct.vkCode == kbd_up) {
            newInput = (newInput & (0xFF ^ NESKEY_UP)) | (downKeys & NESKEY_DOWN);
            downKeys &= 0xFF ^ NESKEY_UP;
        }
        else if (kbdStruct.vkCode == kbd_down) {
            newInput = (newInput & (0xFF ^ NESKEY_DOWN)) | (downKeys & NESKEY_UP);
            downKeys &= 0xFF ^ NESKEY_DOWN;
        }
        else if (kbdStruct.vkCode == kbd_left) {
            newInput = (newInput & (0xFF ^ NESKEY_LEFT)) | (downKeys & NESKEY_RIGHT);
            downKeys &= 0xFF ^ NESKEY_LEFT;
        }
        else if (kbdStruct.vkCode == kbd_right) {
            newInput = (newInput & (0xFF ^ NESKEY_RIGHT)) | (downKeys & NESKEY_LEFT);
            downKeys &= 0xFF ^ NESKEY_RIGHT;
        }
        else if (kbdStruct.vkCode == kbd_a) {
            newInput &= 0xFF ^ NESKEY_A;
            downKeys &= 0xFF ^ NESKEY_A;
        }
        else if (kbdStruct.vkCode == kbd_b) {
            newInput &= 0xFF ^ NESKEY_B;
            downKeys &= 0xFF ^ NESKEY_B;
        }
        else if (kbdStruct.vkCode == kbd_select) {
            newInput &= 0xFF ^ NESKEY_S;
            downKeys &= 0xFF ^ NESKEY_S;
        }
        else if (kbdStruct.vkCode == kbd_start) {
            newInput &= 0xFF ^ NESKEY_T;
            downKeys &= 0xFF ^ NESKEY_T;
        } else {
            return CallNextHookEx(0, nCode, wParam, lParam);
        }
    }

    if (newInput == inputState) {
        return CallNextHookEx(0, nCode, wParam, lParam);
    }
    
    inputState = currentInputs = newInput;

    struct timeb end;
    ftime(&end);
    
    float diff = (float) ((1000.0 * (end.time - currentTimer.time) + (end.millitm - currentTimer.millitm)) / framerate);
    printf("%3.2f\n", diff);
    print_inputstate(newInput);
    currentTimer = end;
    return CallNextHookEx(0, nCode, wParam, lParam);
}

int KBDInit() {
    ftime(&currentTimer);
    SetWindowsHookEx(WH_KEYBOARD_LL, KBDCallback, NULL, 0);
    return 0;
}

int InputReadSetting(void* user, const char* section, const char* name, const char* value) {
    #define SETTING(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (SETTING("NESpy", "inputtype")) snprintf(inputtype, sizeof(inputtype), "%s", value);
    if (SETTING("NES", "comport")) snprintf(comport, sizeof(comport), "%s", value);
    if (SETTING("NES", "ignoreextrareads")) nes_ignoreextrareads = strtol(value, NULL, 10);
    if (SETTING("NES", "suminputs")) nes_suminputs = strtol(value, NULL, 10);

    if (SETTING("JOYPAD", "fps")) joy_framerate = 1000.0f / strtof(value, NULL);
    if (SETTING("JOYPAD", "device")) snprintf(joy_device, sizeof(joy_device), "%s", value);
    if (SETTING("JOYPAD", "debug")) joy_debug = strtol(value, NULL, 10);
    if (SETTING("JOYPAD", "up")) joy_up = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "down")) joy_down = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "left")) joy_left = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "right")) joy_right = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "a")) joy_a = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "b")) joy_b = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "start")) joy_start = joynameToKeyCode(value);
    if (SETTING("JOYPAD", "select")) joy_select = joynameToKeyCode(value);
    
    if (SETTING("KEYBOARD", "fps")) framerate = 1000.0f / strtof(value, NULL);
    if (SETTING("KEYBOARD", "lrmode")) kbd_lrmode = strtol(value, NULL, 10);
    if (SETTING("KEYBOARD", "debug")) kbd_debug = strtol(value, NULL, 10);
    if (SETTING("KEYBOARD", "up")) kbd_up = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "down")) kbd_down = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "left")) kbd_left = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "right")) kbd_right = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "a")) kbd_a = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "b")) kbd_b = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "start")) kbd_start = keynameToKeyCode(value);
    if (SETTING("KEYBOARD", "select")) kbd_select = keynameToKeyCode(value);
    return 0;
}

int InputStartup() {
    if (strcmp(inputtype, "NES") == 0) {
        return NESInit();
    } else if (strcmp(inputtype, "JOYPAD") == 0) {
        return JOYInit();
    } else {
        return KBDInit();
    }
}