#pragma safety enable

/* a call through one parameter changes the object of the other only when they are the same: the warning says so (in case src == dest), and restrict removes it */

struct B { int* p; };
void touch(struct B* b);

/*
    entry flow:

    dest = new();
    if (src == dest) src = dest;
    else             src = new();
*/

/* must warn, labeled with the case */
void call_through_other(struct B* dest, struct B* src)
{
    if (src->p)
    {
        touch(dest);
        *src->p = 1; //lint 100 in case src == dest
    }
}

void use(int* p);

/* must warn, labeled: passing the value read through src */
void call_then_pass(struct B* dest, struct B* src)
{
    if (src->p)
    {
        touch(dest);
        use(src->p); //lint 100 in case src == dest
    }
}

struct F { void (* fn)(void); };
void touch_f(struct F* f);

/* must warn, labeled: calling the function pointer read through src */
void call_then_call(struct F* dest, struct F* src)
{
    if (src->fn)
    {
        touch_f(dest);
        src->fn(); //lint 100 in case src == dest
    }
}

/* Must NOT warn: restrict says dest and src are different objects. */
void call_through_other_restrict(struct B* restrict dest, struct B* restrict src)
{
    if (src->p)
    {
        touch(dest);
        *src->p = 1;
    }
}
