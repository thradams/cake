/*
   The assignments that fill a compound literal are hoisted to statements.
   They must run at the right place:
   - in the declaration of a for, in the init of a for, in the init
     statement and in the declaration-condition of an if or switch;
   - in a declaration with several declarators, after the declarators
     before it ("int a = 1, *p = (int[]){ a };");
   - on the right of a comma operator, after its left operand.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

struct point
{
    int x;
    int y;
};

static int sum(const int* a, int n)
{
    int s = 0;
    for (int i = 0; i < n; i++)
        s += a[i];
    return s;
}

static int value(const struct point* p)
{
    return p->x * 10 + p->y;
}

/* fills the stack with a value that is not zero */
static void dirty(void)
{
    volatile char junk[512];
    for (int i = 0; i < 512; i++)
        junk[i] = 0x7f;
}

static int for_declaration(int n)
{
    int r = 0;

    for (int k = sum((int[]){ 1, 2 }, 2); k < 5; k++)
        r += 1;

    for (int k = 0, m = value(&(struct point){ n, 1 }); k < m; k += 10)
        r += 10;

    for (struct point* p = &(struct point){ 0, 2 }; p->x < p->y; p->x++)
        r += 100;

    return r;
}

static int for_init_expression(int n)
{
    int r = 0;
    int k;

    for (k = sum((int[]){ n, 2 }, 2); k < 5; k++)
        r += 1;

    for (k = value(&(struct point){ 0, n }); k < 3; k++)
    {
        r += 10;
    }

    return r;
}

static int if_and_switch(int n)
{
    int r = 0;

    if (int k = sum((int[]){ n, 2 }, 2); k > 2)
        r += 1;

    if (int k = value(&(struct point){ 0, n }))
        r += 10 * k;

    switch (int k = sum((int[]){ n, 1 }, 2); k)
    {
        case 2:
            r += 100;
            break;
    }

    int j;
    if (j = sum((int[]){ n, 3 }, 2); j == 4)
        r += 1000;

    return r;
}

static int several_declarators(void)
{
    int a = 1, *q = (int[]){ a, a + 1 };
    int b = sum((int[]){ 5 }, 1), c = sum((int[]){ b, 1 }, 2);

    return q[0] + q[1] * 10 + b * 100 + c * 1000;
}

static int comma_operator(void)
{
    int r = 0;
    int k = 0;

    k = sum((int[]){ 1 }, 1), r += sum((int[]){ k, 1 }, 2);
    r += (k = 4, sum((int[]){ k, k }, 2)) * 10;
    r += (k++, k++, value(&(struct point){ k, 0 })) * 100;

    return r;
}

int main(void)
{
    dirty();
    CHECK(for_declaration(1) == 2 + 20 + 200);
    dirty();
    CHECK(for_init_expression(1) == 2 + 20);
    dirty();
    CHECK(if_and_switch(1) == 1 + 10 + 100 + 1000);
    dirty();
    CHECK(several_declarators() == 1 + 20 + 500 + 6000);
    dirty();
    CHECK(comma_operator() == 2 + 80 + 6000);

    return failures;
}
