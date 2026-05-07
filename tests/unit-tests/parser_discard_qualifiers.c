struct X { int i; void* _Opt p; };
void f(struct X* p) {}

int main()
{
    const struct X x = {0};

    //warning:  discarding const at argument

    f(&x); //lint 15
}
