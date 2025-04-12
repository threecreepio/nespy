#include <windows.h>
#include <sys\timeb.h>
#include "nespy.h"
#include "inputs.h"

float kbd_framerate = 16.6393322f;
int inputState = 0;
int downKeys = 0;
int kbd_lrmode, kbd_up, kbd_down, kbd_left, kbd_right, kbd_a, kbd_b, kbd_start, kbd_select;
int kbd_snes_l, kbd_snes_r, kbd_snes_a, kbd_snes_x;
int kbd_debug = 0;

LRESULT __stdcall KBDCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
    int newInput = inputState;
    
    if (wParam == WM_KEYDOWN) {
        if (kbdStruct.vkCode == kbd_up) {
            downKeys |= NESKEY_UP;
            int islr = downKeys & NESKEY_DOWN;
            if (kbd_lrmode == KBDLR_BIZHAWK) {
                newInput &= 0xFFFF ^ (NESKEY_UP | NESKEY_DOWN);
            }
            if (kbd_lrmode == KBDLR_MESEN && islr) {
                newInput &= 0xFFFF ^ NESKEY_DOWN;
            }
            newInput |= NESKEY_UP;
            if (kbd_lrmode == KBDLR_FCEUX && islr) {
                newInput &= 0xFFFF ^ (NESKEY_UP | NESKEY_DOWN);
            }
        }
        else if (kbdStruct.vkCode == kbd_down) {
            downKeys |= NESKEY_DOWN;
            int islr = downKeys & NESKEY_UP;
            if (kbd_lrmode == KBDLR_BIZHAWK) {
                newInput &= 0xFFFF ^ (NESKEY_UP | NESKEY_DOWN);
            }
            if (kbd_lrmode == KBDLR_MESEN && islr) {}
            else newInput |= NESKEY_DOWN;
            if (kbd_lrmode == KBDLR_FCEUX && islr) {
                newInput &= 0xFFFF ^ (NESKEY_UP | NESKEY_DOWN);
            }
        }
        else if (kbdStruct.vkCode == kbd_left) {
            downKeys |= NESKEY_LEFT;
            int islr = downKeys & NESKEY_RIGHT;
            if (kbd_lrmode == KBDLR_BIZHAWK && islr) {
                newInput &= 0xFFFF ^ (NESKEY_LEFT | NESKEY_RIGHT);
            }
            if (kbd_lrmode == KBDLR_MESEN && islr) {
                newInput &= 0xFFFF ^ NESKEY_RIGHT;
            }
            newInput |= NESKEY_LEFT;
            if (kbd_lrmode == KBDLR_FCEUX && islr) {
                newInput &= 0xFFFF ^ (NESKEY_LEFT | NESKEY_RIGHT);
            }
        }
        else if (kbdStruct.vkCode == kbd_right) {
            downKeys |= NESKEY_RIGHT;
            int islr = downKeys & NESKEY_LEFT;
            if (kbd_lrmode == KBDLR_BIZHAWK) {
                newInput &= 0xFFFF ^ (NESKEY_LEFT | NESKEY_RIGHT);
            }
            if (kbd_lrmode == KBDLR_MESEN && islr) {}
            else newInput |= NESKEY_RIGHT;
            if (kbd_lrmode == KBDLR_FCEUX && islr) {
                newInput &= 0xFFFF ^ (NESKEY_LEFT | NESKEY_RIGHT);
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
        }
        else if (kbdStruct.vkCode == kbd_snes_l) {
            downKeys |= NESKEY_SNES_L;
            newInput |= NESKEY_SNES_L;
        }
        else if (kbdStruct.vkCode == kbd_snes_r) {
            downKeys |= NESKEY_SNES_R;
            newInput |= NESKEY_SNES_R;
        }
        else if (kbdStruct.vkCode == kbd_snes_x) {
            downKeys |= NESKEY_SNES_X;
            newInput |= NESKEY_SNES_X;
        }
        else if (kbdStruct.vkCode == kbd_snes_a) {
            downKeys |= NESKEY_SNES_A;
            newInput |= NESKEY_SNES_A;
        } else {
            return CallNextHookEx(0, nCode, wParam, lParam);
        }
    }

    if (wParam == WM_KEYUP) {
        if (kbd_debug) {
            printf("Key: %x\n", (int) kbdStruct.vkCode);
        }
        if (kbdStruct.vkCode == kbd_up) {
            newInput = (newInput & (0xFFFF ^ NESKEY_UP)) | (downKeys & NESKEY_DOWN);
            downKeys &= 0xFFFF ^ NESKEY_UP;
        }
        else if (kbdStruct.vkCode == kbd_down) {
            newInput = (newInput & (0xFFFF ^ NESKEY_DOWN)) | (downKeys & NESKEY_UP);
            downKeys &= 0xFFFF ^ NESKEY_DOWN;
        }
        else if (kbdStruct.vkCode == kbd_left) {
            newInput = (newInput & (0xFFFF ^ NESKEY_LEFT)) | (downKeys & NESKEY_RIGHT);
            downKeys &= 0xFFFF ^ NESKEY_LEFT;
        }
        else if (kbdStruct.vkCode == kbd_right) {
            newInput = (newInput & (0xFFFF ^ NESKEY_RIGHT)) | (downKeys & NESKEY_LEFT);
            downKeys &= 0xFFFF ^ NESKEY_RIGHT;
        }
        else if (kbdStruct.vkCode == kbd_a) {
            newInput &= 0xFFFF ^ NESKEY_A;
            downKeys &= 0xFFFF ^ NESKEY_A;
        }
        else if (kbdStruct.vkCode == kbd_b) {
            newInput &= 0xFFFF ^ NESKEY_B;
            downKeys &= 0xFFFF ^ NESKEY_B;
        }
        else if (kbdStruct.vkCode == kbd_select) {
            newInput &= 0xFFFF ^ NESKEY_S;
            downKeys &= 0xFFFF ^ NESKEY_S;
        }
        else if (kbdStruct.vkCode == kbd_start) {
            newInput &= 0xFFFF ^ NESKEY_T;
            downKeys &= 0xFFFF ^ NESKEY_T;
        } 
        else if (kbdStruct.vkCode == kbd_snes_l) {
            newInput &= 0xFFFF ^ NESKEY_SNES_L;
            downKeys &= 0xFFFF ^ NESKEY_SNES_L;
        }
        else if (kbdStruct.vkCode == kbd_snes_r) {
            newInput &= 0xFFFF ^ NESKEY_SNES_R;
            downKeys &= 0xFFFF ^ NESKEY_SNES_R;
        }
        else if (kbdStruct.vkCode == kbd_snes_x) {
            newInput &= 0xFFFF ^ NESKEY_SNES_X;
            downKeys &= 0xFFFF ^ NESKEY_SNES_X;
        }
        else if (kbdStruct.vkCode == kbd_snes_a) {
            newInput &= 0xFFFF ^ NESKEY_SNES_A;
            downKeys &= 0xFFFF ^ NESKEY_SNES_A;
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
    
    updateInputState(newInput, kbd_framerate, 1);
    return CallNextHookEx(0, nCode, wParam, lParam);
}

int KBDInit() {
    SetWindowsHookEx(WH_KEYBOARD_LL, KBDCallback, NULL, 0);
    return 0;
}

int KBDInputReadSetting(void* user, const char* section, const char* name, const char* value) {
    if (SETTING("KEYBOARD", "fps")) kbd_framerate = 1000.0f / strtof(value, NULL);
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

    if (snesmode) {
        if (SETTING("KEYBOARD", "y")) kbd_a = keynameToKeyCode(value);
        if (SETTING("KEYBOARD", "a")) kbd_snes_a = keynameToKeyCode(value);
        if (SETTING("KEYBOARD", "l")) kbd_snes_l = keynameToKeyCode(value);
        if (SETTING("KEYBOARD", "r")) kbd_snes_r = keynameToKeyCode(value);
        if (SETTING("KEYBOARD", "x")) kbd_snes_x = keynameToKeyCode(value);
    }
    
    return 0;
}
