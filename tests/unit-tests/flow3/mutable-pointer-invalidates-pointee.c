#pragma safety enable

/* passing &x to a plain non-const pointer parameter invalidates the pointee to ANY: `compile_assert(x.i == 5)` after f(&x) must fail */

struct X
{
    int i;
};

void f(struct X* p) {}
void f_const(const struct X* p) {}

struct X make(void);

void mutable_pointer_invalidates_known_value(void)
{
    struct X x = { 5 };
    compile_assert(x.i == 5);
    f(&x);
    /* x.i is no longer trusted to be 5 -- f could have written anything */
    // static_debug(x);
    // compile_assert(x.i == 5); // fails now: value could be any value
}

void const_pointer_does_not_invalidate(void)
{
    struct X x = { 5 };
    f_const(&x);
    /* a pointer-to-const parameter cannot write, so x.i survives the call */
    compile_assert(x.i == 5);
}

void repeated_calls_stay_any(void)
{
    struct X x = { 5 };
    f(&x);
    f(&x); /* still just "any": a second mutating call doesn't un-know it */
    // static_debug(x);
}

struct pair
{
    int a;
    int b;
};

void touch_pair(struct pair* p) {}

void whole_struct_invalidated_recursively(void)
{
    /* the invalidation recurses into every leaf member, same as _Clear */
    struct pair p = { 1, 2 };
    touch_pair(&p);
    // static_debug(p);
    // compile_assert(p.a == 1); // fails: any value
    // compile_assert(p.b == 2); // fails: any value
}
