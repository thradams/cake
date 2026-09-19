#pragma safety enable

/* once any union member is initialized the whole union is: `struct S s = { 0 }` must not report the other union members uninitialized */

struct object
{
    int state;
    union { long long i; unsigned long long u; long double d; } value;
};

void use(struct object o);

/* Zero-init sets the union storage: no member is uninitialized. */
struct object make_zero(void)
{
    struct object empty = { 0 };
    return empty;                    /* clean */
}

/* Writing one union member initializes the union. */
void write_one(void)
{
    struct object o = { 0 };
    o.value.i = 42;
    use(o);                          /* clean: value.u / value.d not flagged */
}

/* documented: a union that is never initialized still warns when used */
