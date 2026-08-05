#pragma safety enable

struct object { int state; union { long long i; unsigned long long u; } value; };

void use(struct object o);

void branched(int is_signed)
{
    struct object o = { 0 };
    o.state = 1;
    if (is_signed)
        o.value.i = 1;
    else
        o.value.u = 2;
    use(o);
}
