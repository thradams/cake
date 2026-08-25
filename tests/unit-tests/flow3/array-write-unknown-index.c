#pragma safety enable

/* 85 is off by default and the folded condition is most directly visible
   through it. */
#pragma cake diagnostic warning 85

/*
   A write through an index that is not pinned to one element invalidates the
   WHOLE array. Before, only the subscript expression's own (temporary) state
   was updated, so the seeds left by an initializer survived the write:

       char buffer[128] = { 0 };
       while (*s) { buffer[c] = *s; c++; s++; }
       if (buffer[1] == 'o' || buffer[1] == 'O')   <- "always false"

   Reduced from src/expressions.c (parse of an octal constant's o/O prefix),
   user-reported.

   Two shapes have to be widened, and they are not the same test:

     - an index whose value is simply unknown (a parameter);
     - an index that IS known inside the loop body, but only because the body
       is analysed with one iteration's state -- a counter the body advances.
       Only a genuine constant expression counts as pinned inside a loop.

   The complementary "a constant index writes just that element" cases live in
   array-elements.c; nothing here should weaken them.
*/

/* Unknown index: every element of the array becomes unknown. */
int unknown_index(const char* s, int c)
{
    char buffer[16] = { 0 };
    buffer[c] = *s;
    if (buffer[1] == 'o' || buffer[1] == 'O')
    {
        return 1;
    }
    return 0;
}

/* Same write inside a loop. */
int unknown_index_in_loop(const char* s, int c)
{
    char buffer[16] = { 0 };
    while (*s)
    {
        buffer[c] = *s;
        s++;
    }
    if (buffer[1] == 'O')
    {
        return 1;
    }
    return 0;
}

/* Index known per iteration only: the counter the body advances. */
int loop_counter_index(const char* s)
{
    int c = 0;
    char buffer[16] = { 0 };
    while (*s)
    {
        buffer[c] = *s;
        c++;
        s++;
    }
    if (buffer[1] == 'o' || buffer[1] == 'O')
    {
        return 1;
    }
    return 0;
}

/* Same, written as a for loop over the loop variable. */
int for_loop_index(int n)
{
    char buffer[16] = { 0 };
    for (int i = 0; i < n; i++)
    {
        buffer[i] = 'x';
    }
    if (buffer[1] == 'O')
    {
        return 1;
    }
    return 0;
}

/* Compound assignment and increment write the element too. */
int compound_and_increment(int c)
{
    int v[4] = { 0, 0, 0, 0 };
    v[c] += 3;
    if (v[1] == 7)
    {
        return 1;
    }

    int w[4] = { 0, 0, 0, 0 };
    w[c]++;
    if (w[2] == 5)
    {
        return 1;
    }
    return 0;
}
