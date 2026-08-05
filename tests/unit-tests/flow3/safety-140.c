
#pragma safety enable


struct X { int i;  };
void f(struct X* p);
void f2(struct X* p);

void delete(struct X* p)
{
    compile_assert(p != 0);
    f(p);
    compile_assert(p != 0);
    f2(p);
}
