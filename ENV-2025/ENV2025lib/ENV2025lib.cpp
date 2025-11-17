#include <iostream>
#include <cstring>
#include <cstdlib>

extern "C" {

    // Функция сравнения строк для ENV-2025
    int __stdcall compare(char* str1, char* str2)
    {
        if (str1 == nullptr || str2 == nullptr)
            return 0;
        return strcmp(str1, str2) == 0 ? 1 : 0;
    }

    // Функция вычисления длины строки для ENV-2025
    int __stdcall length(char* str)
    {
        if (str == nullptr)
            return 0;
        return strlen(str);
    }

    // Вывод целого числа
    int __stdcall outnum(int value)
    {
        std::cout << value;
        return 0;
    }

    // Вывод строки
    int __stdcall outstr(char* ptr)
    {
        if (ptr == nullptr)
            return 0;

        for (int i = 0; ptr[i] != '\0'; i++)
            std::cout << ptr[i];
        return 0;
    }

    // Вывод целого числа с переводом строки
    int __stdcall outnumline(int value)
    {
        std::cout << value << std::endl;
        return 0;
    }

    // Вывод строки с переводом строки
    int __stdcall outstrline(char* ptr)
    {
        if (ptr == nullptr)
        {
            std::cout << std::endl;
            return 0;
        }

        for (int i = 0; ptr[i] != '\0'; i++)
            std::cout << ptr[i];
        std::cout << std::endl;
        return 0;
    }

    // Ожидание нажатия клавиши
    int __stdcall system_pause()
    {
        system("pause");
        return 0;
    }
}