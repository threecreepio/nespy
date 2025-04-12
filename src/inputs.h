#pragma once
#define NESKEY_UP    0x010
#define NESKEY_DOWN  0x020
#define NESKEY_LEFT  0x040
#define NESKEY_RIGHT 0x080
#define NESKEY_A 0x001
#define NESKEY_B 0x002
#define NESKEY_S 0x004
#define NESKEY_T 0x008
#define NESKEY_SNES_A 0x100
#define NESKEY_SNES_X 0x200
#define NESKEY_SNES_L 0x400
#define NESKEY_SNES_R 0x800

#define SOCD_DISABLED 0
#define SOCD_NEUTRAL 1

extern int snesmode;
extern int inputErrorCode;

int joynameToKeyCode(const char *input);
int keynameToKeyCode(const char *input);
void updateInputState(int newInput, float framerate, int estimateframes);
int handleSOCD(int newInputs);

int InputStartup();

int KBDInit();
int NESInit();
int JOYInit();

int InputReadSetting(void* user, const char* section, const char* name, const char* value);
int NESInputReadSetting(void* user, const char* section, const char* name, const char* value);
int JOYInputReadSetting(void* user, const char* section, const char* name, const char* value);
int KBDInputReadSetting(void* user, const char* section, const char* name, const char* value);
