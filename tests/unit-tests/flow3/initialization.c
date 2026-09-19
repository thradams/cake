#pragma safety enable

/* a constant initializer seeds the variable's initial value from the object the parser computed */

/* Scalar: the literal's value is taken straight from the object. */
void scalar(void)
{
    int x = 5;
    compile_assert(x == 5);
}

/* A constant-folded initializer expression is stored as the object's value. */
void folded(void)
{
    int x = 2 + 3;
    compile_assert(x == 5);
}

/* Struct members are seeded individually: pt.x == 1, pt.y == 2. */
struct point { int x, y; };

void aggregate(void)
{
    struct point pt = { 1, 2 };
    compile_assert(pt.x == 1);
    compile_assert(pt.y == 2);
}

/* Initializing from a constant local carries the constant across. */
void from_const_local(void)
{
    int k = 42;
    int x = k;
    compile_assert(x == 42);
}

/* array elements are seeded too: v[i] with a constant index has its initializer value */
void array(void)
{
    int v[3] = { 10, 20, 30 };
    compile_assert(v[0] == 10);
    compile_assert(v[1] == 20);
    compile_assert(v[2] == 30);
}
