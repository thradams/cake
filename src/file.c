#include <stdio.h>
#include <locale.h>

#ifdef _WIN32
#include <Windows.h>
int utf8_and_vt100()
{
    setlocale(LC_ALL, ".UTF8");
    DWORD dwMode = 0;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    return (hOut != INVALID_HANDLE_VALUE &&
            GetConsoleMode(hOut, &dwMode) != 0 &&
            SetConsoleMode(hOut, dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0 &&
            SetConsoleOutputCP(CP_UTF8) != 0);
}
#endif

int main()
{
    utf8_and_vt100();
    printf("Hello, 世界");
}
