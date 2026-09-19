#pragma safety enable

/* a constant index resolves to the element object, so per-element values are tracked like x.f */

/* Element initializers are known per index. */
void init(void)
{
    int v[3] = { 10, 20, 30 };
    compile_assert(v[0] == 10);
    compile_assert(v[1] == 20);
    compile_assert(v[2] == 30);
}

/* Writing an element updates just that element. */
void write(void)
{
    int v[2] = { 0, 0 };
    v[0] = 7;
    compile_assert(v[0] == 7);
    compile_assert(v[1] == 0);
}

/* Arrays of structs: v[i].field resolves through element then member. */
struct point { int x, y; };

void of_structs(void)
{
    struct point p[] = { {1, 2}, {3, 4} };
    compile_assert(p[0].x == 1);
    compile_assert(p[0].y == 2);
    compile_assert(p[1].x == 3);
    compile_assert(p[1].y == 4);
}

/* a non-constant index is unknown by design: compile_assert(v[i] == 1) is not provable */
