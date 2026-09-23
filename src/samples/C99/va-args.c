#include <stdio.h>

#define MYLOG(FormatLiteral, ...)  fprintf (stderr, "%s(%u): " FormatLiteral "\n", __FILE__, __LINE__, __VA_ARGS__)

int main()
{
 MYLOG("Too many balloons %u", 42);
}
