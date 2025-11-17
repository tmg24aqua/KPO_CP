#pragma once

extern "C" {
    int __stdcall compare(char* str1, char* str2);
    int __stdcall length(char* str);
    int __stdcall outnum(int value);
    int __stdcall outstr(char* ptr);
    int __stdcall outnumline(int value);
    int __stdcall outstrline(char* ptr);
    int __stdcall system_pause();
}