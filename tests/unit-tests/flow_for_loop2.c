#pragma safety enable

struct X {
    struct X* _Opt next;
};

struct X* _Opt f();

int c;

int main()
{
    struct X* _Opt p = f();
    for (; p; p = p->next)
    {

    }
    assert_state(p, "null");

    for (p = f(); p; p = p->next)
    {
    }
    assert_state(p, "null");

    for (p = f(); p; )
    {
        p = p->next;
    }
    assert_state(p, "null");

    for (p = f(); p; )
    {
        if (c)
            break;
        p = p->next;
    }
    assert_state(p, "null not-null");

}