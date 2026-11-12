/*
   bool is lowered to an unsigned char, so b++ must keep the value 0 or 1
   (6.5.2.4: b++ is b += 1, and the conversion to bool gives 1).
   An operand with side effects (a[i++] |= x) is evaluated once, even when
   the generated code reads it twice.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static int id(int x)
{
    return x;
}

int main(void)
{
    bool b = id(0) != 0;
    bool old = b++;
    CHECK(old == 0);
    CHECK(b == 1);

    old = b++;
    CHECK(old == 1);
    CHECK(b == 1);
    CHECK((int)b == 1);

    old = b--;
    CHECK(old == 1);
    CHECK(b == 0);

    old = b--;
    CHECK(old == 0);
    CHECK(b == 1);

    /* an operand with side effects is evaluated once */
    bool flags[3] = { false, false, false };
    int index = 0;
    flags[index++] |= true;
    CHECK(index == 1);
    CHECK(flags[0] == 1);
    CHECK(flags[1] == 0);

    bool another[3] = { false, true, false };
    index = 0;
    another[index++]++;
    CHECK(index == 1);
    CHECK(another[0] == 1);
    CHECK(another[1] == 1);

    _BitInt(5) small[3] = { 1, 2, 3 };
    index = 0;
    small[index++] += 4;
    CHECK(index == 1);
    CHECK(small[0] == 5);
    CHECK(small[1] == 2);

    index = 1;
    _BitInt(5) previous = small[index++]++;
    CHECK(index == 2);
    CHECK(previous == 2);
    CHECK(small[1] == 3);

    /* the right operand of a converted compound assignment is a whole expression */
    bool acc = true;
    int two = id(2);
    int three = id(3);
    acc &= two + three;
    CHECK(acc == 1);

    _BitInt(5) w = 1;
    w *= two + three;
    CHECK(w == 5);

    return failures;
}
