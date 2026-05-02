
#pragma safety enable


struct X { int i;  };
void f(struct X* p);
void f2(struct X* p);

void delete(struct X* p)
{
    assert_state(p, "not-null");
    f(p);
    assert_state(p, "not-null");
    f2(p);
}
