/*
   Small cases of the generated code:
   - "- -x" is not "--x";
   - char s[10] = "abc" copies the literal and zeroes the rest;
   - the arguments of a call through a function pointer are converted to the
     parameter type (bool);
   - a label followed by a declaration that generates no statement;
   - a struct with a multidimensional array of structs;
   - volatile on struct, enum and bool objects.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static int id(int x)
{
    return x;
}

/* fills the stack with a value that is not zero */
static void dirty(void)
{
    volatile char junk[256];
    for (int i = 0; i < 256; i++)
        junk[i] = 0x7f;
}

static int check_string(void)
{
    char s[10] = "abc";
    int ok = s[0] == 'a' && s[1] == 'b' && s[2] == 'c';
    for (int i = 3; i < 10; i++)
    {
        if (s[i] != 0)
            ok = 0;
    }
    return ok;
}

struct Named
{
    char name[8];
    int x;
};

static int check_member_string(void)
{
    struct Named n = { .name = "ab", .x = id(1) };
    int ok = n.name[0] == 'a' && n.name[1] == 'b' && n.x == 1;
    for (int i = 2; i < 8; i++)
    {
        if (n.name[i] != 0)
            ok = 0;
    }
    return ok;
}

static int as_int(bool b)
{
    return b ? 100 : 7;
}

static int jump_over_declaration(void)
{
    int v = 0;
    goto L;
    v = 1;
L:
    static int counter;
    v += 2;
    return v;
}

struct Cell
{
    int v;
};

struct Matrix
{
    struct Cell cells[2][3];
};

enum Mode { MODE_A, MODE_B };
struct Reg { int value; };

int main(void)
{
    /* unary signs */
    int x = id(5);
    int a = - -x;
    CHECK(a == 5);
    int b = + +x;
    CHECK(b == 5);

    int y = id(3);
    int c = - --y;
    CHECK(c == -2);
    CHECK(y == 2);
    int d = + ++y;
    CHECK(d == 3);
    CHECK(y == 3);

    /* strings */
    dirty();
    CHECK(check_string());
    dirty();
    CHECK(check_member_string());

    /* call through a function pointer */
    int (*fp)(bool) = as_int;
    int big = id(256);
    CHECK(fp(big) == 100);
    CHECK(fp(0) == 7);

    /* label before a declaration */
    CHECK(jump_over_declaration() == 2);

    /* a multidimensional array of structs inside a struct */
    struct Matrix m = { 0 };
    m.cells[1][2].v = 9;
    m.cells[0][1].v = 4;
    CHECK(m.cells[1][2].v == 9);
    CHECK(m.cells[0][1].v == 4);
    CHECK(m.cells[0][0].v == 0);

    /* volatile objects of struct, enum and bool types */
    volatile struct Reg reg = { 1 };
    reg.value = 2;
    CHECK(reg.value == 2);
    volatile enum Mode mode = MODE_B;
    CHECK(mode == MODE_B);
    volatile bool flag = id(1) != 0;
    CHECK(flag == 1);
    int target = 5;
    int* volatile pointer = &target;
    CHECK(*pointer == 5);

    return failures;
}
