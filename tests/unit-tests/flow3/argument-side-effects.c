#pragma safety enable


struct X
{
    char* _Opt text;
};

void f(struct X* p, char* s);

void deferred_effect(void)
{
    struct X x = { 0 };
    x.text = "a";
    f(&x, x.text); /* clean: x.text read before f runs */
}

void own_effect(void) {
    struct X x = { 0 };
    x.text = "a";
    f(&x, (x.text = 0, x.text)); //lint 35  passing a possible null pointer
}
