#pragma flow enable

struct X {
    struct X* _Opt next;
};

struct X* _Opt f();

int main()
{
    struct X* _Opt p = f();
    for (; p; p = p->next)
    {

    }
    static_debug(p); //should be null

    for (p = f(); p; p = p->next)
    {
    }
    assert_state(p, "null"); //should be null

    for (p = f(); p; )
    {
        p = p->next;
    }
    assert_state(p, "null"); //should be null
}