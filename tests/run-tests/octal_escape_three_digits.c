/*
   An octal escape sequence has one to three octal digits (6.4.4.5): in
   "\1012" the escape is \101 ('A') and '2' is the next character. The size
   of the array and the folded value of its elements depend on it.
*/

#include <stdio.h>

#pragma CAKE diagnostic ignored 44 /* the sizes are constants, checking them is what is tested */

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static const char constant_text[] = "oct\1012";
static char text[] = "oct\1012";
static char zero_then_digit[] = "\0001";
static char short_escapes[] = "\18\1a";

int main(void)
{
    char local[] = "\1012";
    const char* p = "\1012";

    CHECK(sizeof constant_text == 6);
    CHECK(sizeof text == 6);
    CHECK(text[3] == 'A' && text[4] == '2' && text[5] == 0);

    CHECK(sizeof zero_then_digit == 3);
    CHECK(zero_then_digit[0] == 0 && zero_then_digit[1] == '1' && zero_then_digit[2] == 0);

    CHECK(sizeof short_escapes == 5);
    CHECK(short_escapes[0] == 1 && short_escapes[1] == '8' && short_escapes[2] == 1 && short_escapes[3] == 'a');

    CHECK(sizeof local == 3);
    CHECK(local[0] == 'A' && local[1] == '2' && local[2] == 0);
    CHECK(p[0] == 'A' && p[1] == '2' && p[2] == 0);

    return failures;
}
