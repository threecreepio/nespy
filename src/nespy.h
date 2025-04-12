#pragma once
#include <stdio.h>
#define SETTING(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

extern int currentInputs;
extern FILE *logfile;
extern FILE *inputlog;
