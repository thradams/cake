#include <string.h>
enum E { A, B, C, D, E, F };

void f(enum E e)
{
    switch (e)
    {
    case A:
        break;
    case B:
        break;
    default:
        static_assert(_Countof(e) == 6);
    }
}

enum E parse_enum_e(const char* s)
{
    if (strcmp(s, "A") == 0) return A;
    if (strcmp(s, "B") == 0) return B;
    if (strcmp(s, "C") == 0) return C;
    if (strcmp(s, "D") == 0) return D;
    if (strcmp(s, "E") == 0) return E;
    if (strcmp(s, "F") == 0) return F;
    static_assert(_Countof(enum E) == 6);

    return A;
}

int main() { }
