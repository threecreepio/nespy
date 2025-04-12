#include <windows.h>
#include <tchar.h>
#include <sys\timeb.h>
#include <stdio.h>
#include <stdint.h>
#include "nespy.h"
#include "inputs.h"
#include "dinput.h"

char inputtype[50] = "NES";
int snesmode = 0;
int inputErrorCode = 0;
FILE *inputlog;
struct timeb currentTimer;

int currentInputs;
int repeatedInputs = 0;
int sumInputs = 1;
int socd = SOCD_DISABLED;

int handleSOCD(int newInputs) {
    if (socd == SOCD_DISABLED) return newInputs;
    int outInputs = newInputs;
    if ((newInputs & 0b00110000) == 0b00110000) { // U+D
        outInputs &= 0b1111111111001111;
    }
    if ((newInputs & 0b11000000) == 0b11000000) { // L+R
        outInputs &= 0b1111111100111111;
    }
    return outInputs;
}

void updateInputState(int newInput, float framerate, int estimateframes) {
    uint8_t changed = currentInputs != newInput;
    currentInputs = newInput;
    struct timeb now;
    ftime(&now);
    float diff = (float)((1000.0 * (now.time - currentTimer.time) + (now.millitm - currentTimer.millitm)) / framerate);
    if (diff < 0.05f) {
        return; // double read
    }
    if (estimateframes && diff < 1.0f) return;
    currentTimer = now;

    if (!changed) {
        repeatedInputs += 1;
    }
    if (changed && sumInputs) {
        fprintf(inputlog, " : %d\n", repeatedInputs);
        repeatedInputs = 1;
    }
    if (changed || !sumInputs) {
        if (snesmode) {
            fprintf(inputlog, "%s%s%s%s%s%s%s%s%s%s%s%s",
                (newInput & NESKEY_SNES_L) == NESKEY_SNES_L ? "l" : " ",
                (newInput & NESKEY_SNES_R) == NESKEY_SNES_R ? "r" : " ",
                (newInput & NESKEY_SNES_X) == NESKEY_SNES_X ? "X" : " ",
                (newInput & NESKEY_SNES_A) == NESKEY_SNES_A ? "A" : " ",
                (newInput & NESKEY_RIGHT) == NESKEY_RIGHT ? "R" : " ",
                (newInput & NESKEY_LEFT) == NESKEY_LEFT ? "L" : " ",
                (newInput & NESKEY_DOWN) == NESKEY_DOWN ? "D" : " ",
                (newInput & NESKEY_UP) == NESKEY_UP ? "U" : " ",
                (newInput & NESKEY_T) == NESKEY_T ? "T" : " ",
                (newInput & NESKEY_S) == NESKEY_S ? "S" : " ",
                (newInput & NESKEY_B) == NESKEY_B ? "B" : " ",
                (newInput & NESKEY_A) == NESKEY_A ? "Y" : " "
            );
        } else {
            fprintf(inputlog, "%s%s%s%s%s%s%s%s",
                (newInput & NESKEY_RIGHT) == NESKEY_RIGHT ? "R" : " ",
                (newInput & NESKEY_LEFT) == NESKEY_LEFT ? "L" : " ",
                (newInput & NESKEY_DOWN) == NESKEY_DOWN ? "D" : " ",
                (newInput & NESKEY_UP) == NESKEY_UP ? "U" : " ",
                (newInput & NESKEY_T) == NESKEY_T ? "T" : " ",
                (newInput & NESKEY_S) == NESKEY_S ? "S" : " ",
                (newInput & NESKEY_B) == NESKEY_B ? "B" : " ",
                (newInput & NESKEY_A) == NESKEY_A ? "A" : " "
            );
        }
        if (!sumInputs) {
            fprintf(inputlog, "\n");
        }
    }
}

int InputReadSetting(void* user, const char* section, const char* name, const char* value) {
    if (SETTING("NESpy", "inputtype")) snprintf(inputtype, sizeof(inputtype), "%s", value);
    if (SETTING("NESpy", "snesmode")) snesmode = strtol(value, NULL, 10);
    if (SETTING("NESpy", "suminputs")) sumInputs = strtol(value, NULL, 10);
    if (SETTING("NESpy", "socd")) socd = strtol(value, NULL, 10);
    NESInputReadSetting(user, section, name, value);
    JOYInputReadSetting(user, section, name, value);
    KBDInputReadSetting(user, section, name, value);
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