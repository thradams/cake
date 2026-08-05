#pragma safety enable

/*
   Smoke tests for the * and . / -> operators when the operand has no
   tracked alternatives.
*/

struct X { int i; };

void deref_after_guard(int* _Opt p)
{
    if (p)
    {
        int x = *p;
        (void)x;
    }
}

void member_dot(struct X x)
{
    x.i = 1;
    compile_assert(x.i == 1);
}

void member_arrow(struct X* p)
{
    p->i = 2;
    compile_assert(p->i == 2);
}
