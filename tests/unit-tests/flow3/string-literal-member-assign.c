#pragma safety enable

/* a mutable-pointer argument's write effect is applied after all arguments are evaluated, so `f(&x, x.text)` reads x.text at its current non-null value */

struct X
{
    char* _Opt text;
};

void f(struct X* p, char* s);

int main(void)
{
    struct X x = { 0 };
    x.text = "a";        /* string literal -> x.text is non-null */
    f(&x, x.text);       /* clean: x.text read before f can modify *x */
}
