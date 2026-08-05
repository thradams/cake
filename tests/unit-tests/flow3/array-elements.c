#pragma safety enable

/*
   Array element tracking. A subscript v[i] with a CONSTANT index resolves to
   the array's element object, so flow3 tracks each element's value and
   relation -- initializers, writes, and arrays of structs all work. Mirrors
   how member access x.f is handled.

   A non-constant index can't be pinned to one element, so it stays unknown
   (documented at the bottom). Use compile_assert (flow-checked).
*/

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

/*
   A NON-CONSTANT index is not pinned to a single element, so the value is
   unknown (this is correct, not a bug) and is documented rather than asserted:

       void unknown(int i) {
           int v[3] = { 1, 2, 3 };
           compile_assert(v[i] == 1);   // i is unknown: not provable
       }
*/
