#include <string.h>
#include <stdlib.h>
#include <dinput.h>

int joynameToKeyCode(const char* input)
{
    if (0 == strcmp(input, "UP")) return 32;
    if (0 == strcmp(input, "RIGHT")) return 33;
    if (0 == strcmp(input, "DOWN")) return 34;
    if (0 == strcmp(input, "LEFT")) return 35;
    return strtol(input, 0, 10);
}

int keynameToKeyCode(const char* input)
{
    // be prepared for some beautiful code!
    if (0 == strcmp(input, "ESCAPE")) return DIK_ESCAPE;
    if (0 == strcmp(input, "1")) return DIK_1;
    if (0 == strcmp(input, "2")) return DIK_2;
    if (0 == strcmp(input, "3")) return DIK_3;
    if (0 == strcmp(input, "4")) return DIK_4;
    if (0 == strcmp(input, "5")) return DIK_5;
    if (0 == strcmp(input, "6")) return DIK_6;
    if (0 == strcmp(input, "7")) return DIK_7;
    if (0 == strcmp(input, "8")) return DIK_8;
    if (0 == strcmp(input, "9")) return DIK_9;
    if (0 == strcmp(input, "0")) return DIK_0;
    if (0 == strcmp(input, "MINUS")) return DIK_MINUS;
    if (0 == strcmp(input, "EQUALS")) return DIK_EQUALS;
    if (0 == strcmp(input, "BACK")) return DIK_BACK;
    if (0 == strcmp(input, "TAB")) return DIK_TAB;
    if (0 == strcmp(input, "Q")) return DIK_Q;
    if (0 == strcmp(input, "W")) return DIK_W;
    if (0 == strcmp(input, "E")) return DIK_E;
    if (0 == strcmp(input, "R")) return DIK_R;
    if (0 == strcmp(input, "T")) return DIK_T;
    if (0 == strcmp(input, "Y")) return DIK_Y;
    if (0 == strcmp(input, "U")) return DIK_U;
    if (0 == strcmp(input, "I")) return DIK_I;
    if (0 == strcmp(input, "O")) return DIK_O;
    if (0 == strcmp(input, "P")) return DIK_P;
    if (0 == strcmp(input, "LBRACKET")) return DIK_LBRACKET;
    if (0 == strcmp(input, "RBRACKET")) return DIK_RBRACKET;
    if (0 == strcmp(input, "RETURN")) return DIK_RETURN;
    if (0 == strcmp(input, "LCONTROL")) return DIK_LCONTROL;
    if (0 == strcmp(input, "A")) return DIK_A;
    if (0 == strcmp(input, "S")) return DIK_S;
    if (0 == strcmp(input, "D")) return DIK_D;
    if (0 == strcmp(input, "F")) return DIK_F;
    if (0 == strcmp(input, "G")) return DIK_G;
    if (0 == strcmp(input, "H")) return DIK_H;
    if (0 == strcmp(input, "J")) return DIK_J;
    if (0 == strcmp(input, "K")) return DIK_K;
    if (0 == strcmp(input, "L")) return DIK_L;
    if (0 == strcmp(input, "SEMICOLON")) return DIK_SEMICOLON;
    if (0 == strcmp(input, "APOSTROPHE")) return DIK_APOSTROPHE;
    if (0 == strcmp(input, "GRAVE")) return DIK_GRAVE;
    if (0 == strcmp(input, "LSHIFT")) return DIK_LSHIFT;
    if (0 == strcmp(input, "BACKSLASH")) return DIK_BACKSLASH;
    if (0 == strcmp(input, "Z")) return DIK_Z;
    if (0 == strcmp(input, "X")) return DIK_X;
    if (0 == strcmp(input, "C")) return DIK_C;
    if (0 == strcmp(input, "V")) return DIK_V;
    if (0 == strcmp(input, "B")) return DIK_B;
    if (0 == strcmp(input, "N")) return DIK_N;
    if (0 == strcmp(input, "M")) return DIK_M;
    if (0 == strcmp(input, "COMMA")) return DIK_COMMA;
    if (0 == strcmp(input, "PERIOD")) return DIK_PERIOD;
    if (0 == strcmp(input, "SLASH")) return DIK_SLASH;
    if (0 == strcmp(input, "RSHIFT")) return DIK_RSHIFT;
    if (0 == strcmp(input, "MULTIPLY")) return DIK_MULTIPLY;
    if (0 == strcmp(input, "LMENU")) return DIK_LMENU;
    if (0 == strcmp(input, "SPACE")) return DIK_SPACE;
    if (0 == strcmp(input, "CAPITAL")) return DIK_CAPITAL;
    if (0 == strcmp(input, "F1")) return DIK_F1;
    if (0 == strcmp(input, "F2")) return DIK_F2;
    if (0 == strcmp(input, "F3")) return DIK_F3;
    if (0 == strcmp(input, "F4")) return DIK_F4;
    if (0 == strcmp(input, "F5")) return DIK_F5;
    if (0 == strcmp(input, "F6")) return DIK_F6;
    if (0 == strcmp(input, "F7")) return DIK_F7;
    if (0 == strcmp(input, "F8")) return DIK_F8;
    if (0 == strcmp(input, "F9")) return DIK_F9;
    if (0 == strcmp(input, "F10")) return DIK_F10;
    if (0 == strcmp(input, "NUMLOCK")) return DIK_NUMLOCK;
    if (0 == strcmp(input, "SCROLL")) return DIK_SCROLL;
    if (0 == strcmp(input, "NUMPAD7")) return DIK_NUMPAD7;
    if (0 == strcmp(input, "NUMPAD8")) return DIK_NUMPAD8;
    if (0 == strcmp(input, "NUMPAD9")) return DIK_NUMPAD9;
    if (0 == strcmp(input, "SUBTRACT")) return DIK_SUBTRACT;
    if (0 == strcmp(input, "NUMPAD4")) return DIK_NUMPAD4;
    if (0 == strcmp(input, "NUMPAD5")) return DIK_NUMPAD5;
    if (0 == strcmp(input, "NUMPAD6")) return DIK_NUMPAD6;
    if (0 == strcmp(input, "ADD")) return DIK_ADD;
    if (0 == strcmp(input, "NUMPAD1")) return DIK_NUMPAD1;
    if (0 == strcmp(input, "NUMPAD2")) return DIK_NUMPAD2;
    if (0 == strcmp(input, "NUMPAD3")) return DIK_NUMPAD3;
    if (0 == strcmp(input, "NUMPAD0")) return DIK_NUMPAD0;
    if (0 == strcmp(input, "DECIMAL")) return DIK_DECIMAL;
    if (0 == strcmp(input, "OEM_102")) return DIK_OEM_102;
    if (0 == strcmp(input, "F11")) return DIK_F11;
    if (0 == strcmp(input, "F12")) return DIK_F12;
    if (0 == strcmp(input, "F13")) return DIK_F13;
    if (0 == strcmp(input, "F14")) return DIK_F14;
    if (0 == strcmp(input, "F15")) return DIK_F15;
    if (0 == strcmp(input, "KANA")) return DIK_KANA;
    if (0 == strcmp(input, "ABNT_C1")) return DIK_ABNT_C1;
    if (0 == strcmp(input, "CONVERT")) return DIK_CONVERT;
    if (0 == strcmp(input, "NOCONVERT")) return DIK_NOCONVERT;
    if (0 == strcmp(input, "YEN")) return DIK_YEN;
    if (0 == strcmp(input, "ABNT_C2")) return DIK_ABNT_C2;
    if (0 == strcmp(input, "NUMPADEQUALS")) return DIK_NUMPADEQUALS;
    if (0 == strcmp(input, "PREVTRACK")) return DIK_PREVTRACK;
    if (0 == strcmp(input, "AT")) return DIK_AT;
    if (0 == strcmp(input, "COLON")) return DIK_COLON;
    if (0 == strcmp(input, "UNDERLINE")) return DIK_UNDERLINE;
    if (0 == strcmp(input, "KANJI")) return DIK_KANJI;
    if (0 == strcmp(input, "STOP")) return DIK_STOP;
    if (0 == strcmp(input, "AX")) return DIK_AX;
    if (0 == strcmp(input, "UNLABELED")) return DIK_UNLABELED;
    if (0 == strcmp(input, "NEXTTRACK")) return DIK_NEXTTRACK;
    if (0 == strcmp(input, "NUMPADENTER")) return DIK_NUMPADENTER;
    if (0 == strcmp(input, "RCONTROL")) return DIK_RCONTROL;
    if (0 == strcmp(input, "MUTE")) return DIK_MUTE;
    if (0 == strcmp(input, "CALCULATOR")) return DIK_CALCULATOR;
    if (0 == strcmp(input, "PLAYPAUSE")) return DIK_PLAYPAUSE;
    if (0 == strcmp(input, "MEDIASTOP")) return DIK_MEDIASTOP;
    if (0 == strcmp(input, "VOLUMEDOWN")) return DIK_VOLUMEDOWN;
    if (0 == strcmp(input, "VOLUMEUP")) return DIK_VOLUMEUP;
    if (0 == strcmp(input, "WEBHOME")) return DIK_WEBHOME;
    if (0 == strcmp(input, "NUMPADCOMMA")) return DIK_NUMPADCOMMA;
    if (0 == strcmp(input, "DIVIDE")) return DIK_DIVIDE;
    if (0 == strcmp(input, "SYSRQ")) return DIK_SYSRQ;
    if (0 == strcmp(input, "RMENU")) return DIK_RMENU;
    if (0 == strcmp(input, "PAUSE")) return DIK_PAUSE;
    if (0 == strcmp(input, "HOME")) return DIK_HOME;
    if (0 == strcmp(input, "UP")) return DIK_UP;
    if (0 == strcmp(input, "PRIOR")) return DIK_PRIOR;
    if (0 == strcmp(input, "LEFT")) return DIK_LEFT;
    if (0 == strcmp(input, "RIGHT")) return DIK_RIGHT;
    if (0 == strcmp(input, "END")) return DIK_END;
    if (0 == strcmp(input, "DOWN")) return DIK_DOWN;
    if (0 == strcmp(input, "NEXT")) return DIK_NEXT;
    if (0 == strcmp(input, "INSERT")) return DIK_INSERT;
    if (0 == strcmp(input, "DELETE")) return DIK_DELETE;
    if (0 == strcmp(input, "LWIN")) return DIK_LWIN;
    if (0 == strcmp(input, "RWIN")) return DIK_RWIN;
    if (0 == strcmp(input, "APPS")) return DIK_APPS;
    if (0 == strcmp(input, "POWER")) return DIK_POWER;
    if (0 == strcmp(input, "SLEEP")) return DIK_SLEEP;
    if (0 == strcmp(input, "WAKE")) return DIK_WAKE;
    if (0 == strcmp(input, "WEBSEARCH")) return DIK_WEBSEARCH;
    if (0 == strcmp(input, "WEBFAVORITES")) return DIK_WEBFAVORITES;
    if (0 == strcmp(input, "WEBREFRESH")) return DIK_WEBREFRESH;
    if (0 == strcmp(input, "WEBSTOP")) return DIK_WEBSTOP;
    if (0 == strcmp(input, "WEBFORWARD")) return DIK_WEBFORWARD;
    if (0 == strcmp(input, "WEBBACK")) return DIK_WEBBACK;
    if (0 == strcmp(input, "MYCOMPUTER")) return DIK_MYCOMPUTER;
    if (0 == strcmp(input, "MAIL")) return DIK_MAIL;
    if (0 == strcmp(input, "MEDIASELECT")) return DIK_MEDIASELECT;
    if (0 == strcmp(input, "BACKSPACE")) return DIK_BACKSPACE;
    if (0 == strcmp(input, "NUMPADSTAR")) return DIK_NUMPADSTAR;
    if (0 == strcmp(input, "LALT")) return DIK_LALT;
    if (0 == strcmp(input, "CAPSLOCK")) return DIK_CAPSLOCK;
    if (0 == strcmp(input, "NUMPADMINUS")) return DIK_NUMPADMINUS;
    if (0 == strcmp(input, "NUMPADPLUS")) return DIK_NUMPADPLUS;
    if (0 == strcmp(input, "NUMPADPERIOD")) return DIK_NUMPADPERIOD;
    if (0 == strcmp(input, "NUMPADSLASH")) return DIK_NUMPADSLASH;
    if (0 == strcmp(input, "RALT")) return DIK_RALT;
    if (0 == strcmp(input, "UPARROW")) return DIK_UPARROW;
    if (0 == strcmp(input, "PGUP")) return DIK_PGUP;
    if (0 == strcmp(input, "LEFTARROW")) return DIK_LEFTARROW;
    if (0 == strcmp(input, "RIGHTARROW")) return DIK_RIGHTARROW;
    if (0 == strcmp(input, "DOWNARROW")) return DIK_DOWNARROW;
    if (0 == strcmp(input, "PGDN")) return DIK_PGDN;
    if (0 == strcmp(input, "$")) {
        return strtol(&input[1], 0, 16);
    }
    return 0;
}
