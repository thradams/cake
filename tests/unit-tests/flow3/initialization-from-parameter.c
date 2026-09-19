#pragma safety enable

/* initialization from a non-constant source copies what is known about the source, including its narrowing and branch origin */

/* A narrowed parameter's relation carries into the copy. */
void carries_relation(int p)
{
    if (p > 0)
    {
        int x = p;                 /* x inherits p's ">0" */
        compile_assert(x > 0);
    }
}

/* An equality narrowing carries the concrete value across. */
void carries_value(int p)
{
    if (p == 5)
    {
        int x = p;
        compile_assert(x == 5);
    }
}

/* Initializing from a constant-per-branch parameter stays correlated. */
void chain_of_locals(int p)
{
    if (p == 7)
    {
        int a = p;                 /* a == 7 */
        int b = a;                 /* b inherits a == 7 */
        compile_assert(b == 7);
    }
}

/* an unconstrained parameter carries no value, so `int x = p; compile_assert(x == p)` is not provable */
