/*
   W_SET_BUT_NOT_USED (84) and W_PARAM_SET_BUT_NOT_USED (83): assigned at
   least once and never read back.

   Both are off by default, so this file enables them explicitly.
*/
#pragma CAKE diagnostic warning 83
#pragma CAKE diagnostic warning 84

int side(void);
void take(int* p);

/* ---------- reported ---------- */

void assign_only(void)
{
    int n = 0;
    n = side();
} //lint 84

void increment_statement(void)
{
    int n = 0;
    n++;
} //lint 84

void compound_assign(void)
{
    int n = 0;
    n += side();
} //lint 84

void many_writes_no_read(void)
{
    int n;
    n = 1;
    n = 2;
    n = 3;
} //lint 84

void comma_writes(void)
{
    int a = 0;
    int b = 0;
    a++, b++;
} //lint 84 84

void parameter_set(int x)
{
    x = side();
} //lint 83

/* ---------- not reported ---------- */

/* Read after the write. */
int read_after(void)
{
    int n = 0;
    n = side();
    return n;
}

/* `i < limit` is a genuine read, so the `i++` in the third clause is not
   the only thing touching i. */
int loop_counter(int limit)
{
    int i;
    int total = 0;
    for (i = 0; i < limit; i++)
    {
        total = total + i;
    }
    return total;
}

/* An increment whose RESULT is consumed reads the old value. */
int increment_result_used(int n)
{
    int hits = 0;
    while (n--)
    {
        hits = hits + 1;
    }
    return hits;
}

int compare_after_increment(void)
{
    int count = 0;
    int left = 4;
    do
    {
        left = left - 1;
    } while (left && count++ < 3);
    return left;
}

/* The address escaping means a read can happen out of sight. */
void address_taken(void)
{
    int n = 0;
    n = 1;
    take(&n);
}

/* Writing a volatile object is the point. */
void volatile_write(void)
{
    volatile int n = 0;
    n = 1;
}

/* Only the declarator an lvalue NAMES is a set: these are uses of a / s / p. */
void array_element(void)
{
    int a[3];
    a[0] = 1;
}

void struct_member(void)
{
    struct S { int f; } s;
    s.f = 1;
}

void through_pointer(int* p)
{
    *p = 1;
}

/* sizeof counts as a read. */
void sizeof_is_a_read(void)
{
    int n = 0;
    n = 1;
    (void)sizeof(n);
}

int main(void)
{
    return 0;
}
