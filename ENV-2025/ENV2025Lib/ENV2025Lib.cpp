#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>

extern "C" {
    int __stdcall compare(char* str1, char* str2)
    {
        if (str1 == nullptr || str2 == nullptr)
            return 0;
        int result = strcmp(str1, str2);
        if (result < 0) return -1;
        if (result > 0) return 1;
        return 0;
    }

    int __stdcall lenght(char* str)
    {
        if (str == nullptr)
            return 0;
        return strlen(str);
    }

    int __stdcall outnum(int value)
    {
        std::cout << value;
        return 0;
    }

    int __stdcall outstr(char* ptr)
    {
        if (ptr == nullptr)
            return 0;

        for (int i = 0; ptr[i] != '\0'; i++)    
            std::cout << ptr[i];
        return 0;
    }

    int __stdcall outnumline(int value)
    {
        std::cout << value << std::endl;
        return 0;
    }

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

    int __stdcall system_pause()
    {
        system("pause");
        return 0;
    }

    int __stdcall random(int min, int max)
    {
        static bool initialized = false;
        if (!initialized) {
            srand((unsigned int)time(NULL));
            initialized = true;
        }
        if (min >= max)
            return min;
        return min + (rand() % (max - min + 1));
    }

    int __stdcall power(int base, int exponent)
    {
        if (exponent == 0)
            return 1;
        if (exponent < 0)
            return 0; 
        
        int result = 1;
        for (int i = 0; i < exponent; i++)
        {
            result *= base;
        }
        return result;
    }
}