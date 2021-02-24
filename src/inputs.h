#pragma once
int currentInputs;
int keynameToKeyCode(const char *input);

int InputReadSetting(void* user, const char* section, const char* name, const char* value);
int InputStartup();

