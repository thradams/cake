#pragma safety enable

/*
  returns a non-null pointer
*/
int* f();

int main()
{
    int* p;

    if ((p = f()) == 0) // pointer is always non-null
    {
        /* Now correctly proven dead: an assignment embedded in a
           condition, `(p = f()) == 0`, used to be untracked by the
           enclosing `==` (the assignment expression's own value was
           never seeded into the flow map at all), so this branch was
           treated as ordinarily reachable. Since f() is assumed to
           return a non-null pointer, and that assignment's result is
           now correctly resolved back to p's real state, the condition
           folds to always-false and this whole branch is unreachable. */
        compile_assert(p != 0); // unreachable code
    } //lint 68 unreachable code
    else
    {
        compile_assert(p != 0);
    }
}
