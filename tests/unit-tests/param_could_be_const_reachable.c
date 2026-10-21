/* warning 82 is suggested only when const would not break what callers assume (warning 120) */

struct Y { int value; };
struct X { int i; struct Y * p; };

void f0(const struct Y * y);
void f1(struct Y * y);

/* reads only: could be const */
int reads(struct X * x)
{
    return x->p->value;
} //lint 82

/* writes below the pointee: const would be warning 120, no suggestion */
void writes_below(struct X * x)
{
    x->p->value = 1;
}

/* passes reachable data to a non-const parameter: no suggestion */
void forwards_mutable(struct X * x)
{
    f1(x->p);
}

/* passes reachable data to a const parameter: could be const */
void forwards_const(struct X * x)
{
    f0(x->p);
} //lint 82
