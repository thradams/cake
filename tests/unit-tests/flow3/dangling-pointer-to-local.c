#pragma safety enable

/* p = &x of a block-scoped local dangles after the block (warning 31); the aggregate's own entry must be marked ENDED, not only its members */

struct X
{
    int i;
};

/* escapes through one branch only: p->i is dangling or null either way */
void f(int condition)
{
   struct X * _Opt p = 0;
   if (condition)
   {
     struct X x = {};
     p = &x;
   }
   p->i = 1; //lint 31 33 -> operator: pointed object lifetime has ended, or p is null
}

/* Escapes through BOTH branches -- p is never null here, only dangling. */
void both_branches_escape(int condition)
{
    struct X* _Opt p = 0;
    if (condition)
    {
        struct X x = {};
        p = &x;
    }
    else
    {
        struct X x2 = {};
        p = &x2;
    }
    p->i = 1; //lint 31 operator -> applied to 'p->i': pointed object lifetime has ended
}

/* Contrast: no escape past the local's own scope -- ordinary and safe. */
void no_escape(void)
{
    struct X x = {};
    struct X* p = &x;
    p->i = 1;
}
