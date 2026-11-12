/*
   The condition and the increment of a loop run at every iteration. The
   code they need before them (the body of a statement expression, the old
   value of p++ on a pointer to a VLA...) used to be emitted once, before the
   loop. A loop body that is not a block (while (c) stmt;) must stay one
   statement when the statement generates more than one.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

int main(void)
{
    /* while: the hoisted code of the condition runs at every test */
    int n = 0;
    int iterations = 0;
    while (({ int t = n; n++; t; }) < 3)
        iterations++;
    CHECK(iterations == 3);
    CHECK(n == 4);

    /* do ... while */
    n = 0;
    iterations = 0;
    do
        iterations++;
    while (({ int t = n; n++; t; }) < 3);
    CHECK(iterations == 4);
    CHECK(n == 4);

    /* do ... while with continue: continue goes to the test */
    n = 0;
    iterations = 0;
    int skipped = 0;
    do
    {
        iterations++;
        if (iterations == 2)
        {
            skipped++;
            continue;
        }
    }
    while (({ int t = n; n++; t; }) < 3);
    CHECK(iterations == 4);
    CHECK(skipped == 1);

    /* for: hoisted code in the condition */
    int count = 0;
    for (int i = 0; ({ int limit = 3; i < limit; }); i++)
        count++;
    CHECK(count == 3);

    /* for: hoisted code in the increment, with continue */
    int sum = 0;
    int k = 0;
    for (int i = 0; i < 4; i += ({ int s = 1; s; }))
    {
        sum += i;
        if (i == 1)
            continue;
        k++;
    }
    CHECK(sum == 6);
    CHECK(k == 3);

    /* for without declaration */
    int j;
    int steps = 0;
    for (j = 0; j < 5; j += ({ int s = 2; s; }))
        steps++;
    CHECK(steps == 3);
    CHECK(j == 6);

    /* the old value of p++ on a pointer to a VLA, in the increment */
    int m = 3;
    int grid[3][3] = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
    int (*q)[m] = grid;
    int total = 0;
    for (int row = 0; row < 3; row++, q++)
        total += (*q)[0];
    CHECK(total == 12);

    /* a body that is not a block and generates more than one statement */
    n = 0;
    while (n < 3)
        n += ({ int s = 1; s; });
    CHECK(n == 3);

    n = 0;
    do
        n += ({ int s = 2; s; });
    while (n < 5);
    CHECK(n == 6);

    return failures;
}
