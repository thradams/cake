#pragma safety enable

struct X {
    struct X* _Opt next;
};


void f(struct X* p);
struct X* _Opt get();

int c;

int main()
{
    struct X* _Opt p = get();
    assert_state(p, "null not-null");
    for (; p; p = p->next)
    {
        assert_state(p, "not-null");
    }
    assert_state(p, "null");

    for (p = get(); p; p = p->next)
    {
        assert_state(p, "not-null");
    }
    assert_state(p, "null");

    for (p = get(); p; )
    {
        assert_state(p, "not-null");
        p = p->next;
    }
    assert_state(p, "null");

    for (p = get(); p; )
    {
        if (c)
        {
            assert_state(p, "not-null");
            break;
        }
        p = p->next;
    }
    assert_state(p, "null not-null");

    for (struct X* _Opt p2 = get(); p2; )
    {
        assert_state(p2, "not-null");
        f(p2);
    }

    
    struct X* _Opt p3 = get();
    for (;;)
    {
        break;
    }
    assert_state(p3, "null not-null");

}
