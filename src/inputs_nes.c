#include <windows.h>
#include <stdint.h>
#include <sys\timeb.h>
#include "nespy.h"
#include "inputs.h"

char comport[50] = "";

DWORD WINAPI NESThread(void* data);
int NESInit()
{
    HANDLE thread = CreateThread(NULL, 0, NESThread, NULL, 0, NULL);
    if (!thread) {
        fprintf(logfile, "could not spawn thread\n");
        return -2;
    }
    return 0;
}

DWORD WINAPI NESThread(void* data)
{
    HANDLE nes_comport = NULL;
    while (1) {
        inputErrorCode = 1;
        nes_comport = CreateFile(comport, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (nes_comport == INVALID_HANDLE_VALUE) {
            fprintf(logfile, "could not connect to nes controller on %s\n", comport);
            Sleep(2500);
            continue;
        }

        DCB param = { 0 };
        param.DCBlength = sizeof(param);
        int status = GetCommState(nes_comport, &param);
        if (status == 0) {
            unsigned long errn = GetLastError();
            fprintf(logfile, "could not read nes controller state on %s - status %ld\n", comport, errn);
            CloseHandle(comport);
            Sleep(2500);
            continue;
        }

        param.BaudRate = CBR_115200;
        param.ByteSize = 8;
        param.StopBits = ONESTOPBIT;
        param.Parity = NOPARITY;
        SetCommState(nes_comport, &param);
        status = SetCommMask(nes_comport, EV_RXCHAR);
        if (status == 0) {
            unsigned long errn = GetLastError();
            fprintf(logfile, "could not set nes controller state on %s - status %ld\n", comport, errn);
            CloseHandle(nes_comport);
            Sleep(2500);
            continue;
        }

        DWORD readlen = 0;
        uint8_t b1, b2, b3, b4;
        inputErrorCode = 0;
        while (1) {
            if (!ReadFile(nes_comport, &b1, 1, &readlen, NULL)) {
                break; // failed to read - attempt to reconnect.
            }
            if (!readlen) {
                continue;
            }
            if ((b1 & 0b10000000) == 0) {
                continue; // wait for high bit set, indicates start of input.
            }
            if (!ReadFile(nes_comport, &b2, 1, &readlen, NULL)) {
                break; // failed to read - attempt to reconnect.
            }
            if (!readlen) {
                continue;
            }
            if (!ReadFile(nes_comport, &b3, 1, &readlen, NULL)) {
                break; // failed to read - attempt to reconnect.
            }
            if (!readlen) {
                continue;
            }
            if (!ReadFile(nes_comport, &b4, 1, &readlen, NULL)) {
                break; // failed to read - attempt to reconnect.
            }
            if (!readlen) {
                continue;
            }
            updateInputState(b1 & 0xF | (b2 << 4) | (b3 << 8) | (b4 << 12), 0);
        }

        // if we fall out to here, we need to reconnect to the device
        inputErrorCode = 1;
        Sleep(2500);
        CloseHandle(nes_comport);
    }
    return 0;
}

int NESInputReadSetting(void* user, const char* section, const char* name, const char* value)
{
    if (SETTING("NES", "comport"))
        snprintf(comport, sizeof(comport), "%s", value);
    return 0;
}
