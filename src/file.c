#pragma safety enable

struct X { struct X* next; };
struct X* _Opt f();

int main()
{
    struct X* _Opt p = f();
    for (; p != 0; p = p->next)
    {
        *p = 1;
    }


    for (struct X* _Opt p2 = f(); p2 != 0; p2 = p->next)
    {
        *p2 = 1;
    }
}