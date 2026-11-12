/*
   defer inside the body of a while loop runs once per iteration: at the end
   of the body, at continue, at break and at a return out of the loop.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static int end_of_body(int n)
{
    int r = 0;
    while (n-- > 0)
    {
        defer r++;
    }
    return r;
}

static int with_continue_and_break(int n)
{
    int r = 0;
    while (n-- > 0)
    {
        defer r++;
        if (n == 3)
            continue;
        if (n == 1)
            break;
        r += 10;
    }
    return r;
}

static int with_return(int n, int* p)
{
    while (n-- > 0)
    {
        defer (*p)++;
        if (n == 1)
            return *p;
    }
    return -1;
}

static int continue_inside_switch(int n)
{
    int r = 0;
    while (n-- > 0)
    {
        defer r++;
        switch (n)
        {
            case 2:
                continue;
            case 1:
                break;
        }
        r += 10;
    }
    return r;
}

static int nested(int n)
{
    int r = 0;
    while (n-- > 0)
    {
        defer r += 100;
        int m = 2;
        while (m-- > 0)
        {
            defer r++;
        }
    }
    return r;
}

int main(void)
{
    CHECK(end_of_body(3) == 3);
    CHECK(with_continue_and_break(5) == 24);

    int count = 0;
    CHECK(with_return(4, &count) == 2);
    CHECK(count == 3);

    CHECK(continue_inside_switch(3) == 23);
    CHECK(nested(2) == 204);

    return failures;
}
