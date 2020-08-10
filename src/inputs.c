
#include <windows.h>
#include <sys\timeb.h>
#include <stdio.h>
#include <stdint.h>
#include "nespy.h"
#include "inputs.h"

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
char comport[50] = "\0";
float nes_framemultiplier = 1;
float framerate = 16.6393322f;
int downKeys = 0;
int inputState = 0;
int repeatedInputs = 0;
struct timeb currentTimer;

void print_inputstate(int newInput) {
    printf("%s%s%s%s%s%s%s%s : ",
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

int NESInit();
DWORD WINAPI NESThread(void* data) {
    uint8_t nextByte;
    DWORD readlen;
    while (1) {
            if (!ReadFile( comport, &nextByte, sizeof(nextByte), &readlen, NULL)) {
                Sleep(2500);
                NESInit();
            }
            if (!readlen) continue;
            if (currentInputs == nextByte) {
                repeatedInputs += 1;
            } else {
                printf("%i\n", (int)(repeatedInputs * nes_framemultiplier));
                repeatedInputs = 1;
                print_inputstate(nextByte);
            }
            currentInputs = nextByte;
    }
    return 0;
}

int NESInit() {
    nes_comport = CreateFile(comport, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (nes_comport == INVALID_HANDLE_VALUE) {
        fprintf(logfile, "could not connect to nes controller\n");
        return -1;
    }

    DCB param = { 0 };
    param.DCBlength = sizeof(param);
    int status = GetCommState(comport, &param);
    if (!status) {
        fprintf(logfile, "could not read nes controller state\n");
        CloseHandle(comport);
        return -1;
    }

    param.BaudRate = CBR_115200;
    param.ByteSize = 8;
    param.StopBits = ONESTOPBIT;
    param.Parity   = NOPARITY;
    SetCommState(comport, &param);
    status = SetCommMask(comport, EV_RXCHAR);
    if (!status) {
        fprintf(logfile, "could not set nes controller state\n");
        CloseHandle(comport);
        return -1;
    }

    HANDLE thread = CreateThread(NULL, 0, NESThread, NULL, 0, NULL);
    if (!thread) {
        fprintf(logfile, "could not spawn thread\n");
        CloseHandle(comport);
        return -2;
    }

	return 0;
}

int kbd_lrmode, kbd_up, kbd_down, kbd_left, kbd_right, kbd_a, kbd_b, kbd_start, kbd_select;
int kbd_printinput = 0;
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
        if (kbd_printinput) {
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

int InputReadSetting(void* user, const char* section, const char* name, const char* value) {
    #define SETTING(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (SETTING("NESpy", "inputtype")) snprintf(inputtype, sizeof(inputtype), "%s", value);
    if (SETTING("NES", "comport")) snprintf(comport, sizeof(comport), "%s", value);
    if (SETTING("NES", "framemultiplier")) nes_framemultiplier = strtof(value, NULL);
    if (SETTING("Keyboard", "fps")) framerate = 1000.0f / strtof(value, NULL);
    if (SETTING("Keyboard", "lrmode")) kbd_lrmode = strtol(value, NULL, 10);
    if (SETTING("Keyboard", "print_input")) kbd_printinput = strtol(value, NULL, 10);
    if (SETTING("Keyboard", "up")) kbd_up = strtol(value, NULL, 16);
    if (SETTING("Keyboard", "down")) kbd_down = strtol(value, NULL, 16);
    if (SETTING("Keyboard", "left")) kbd_left = strtol(value, NULL, 16);
    if (SETTING("Keyboard", "right")) kbd_right = strtol(value, NULL, 16);
    if (SETTING("Keyboard", "a")) kbd_a = strtol(value, NULL, 16);
    if (SETTING("Keyboard", "b")) kbd_b = strtol(value, NULL, 16);
    if (SETTING("Keyboard", "start")) kbd_start = strtol(value, NULL, 16);
    if (SETTING("Keyboard", "select")) kbd_select = strtol(value, NULL, 16);
    return 0;
}

int InputStartup() {
    if (strcmp(inputtype, "NES") == 0) {
        return NESInit();
    } else {
        ftime(&currentTimer);
        SetWindowsHookEx(WH_KEYBOARD_LL, KBDCallback, NULL, 0);
        return 0;
    }
}