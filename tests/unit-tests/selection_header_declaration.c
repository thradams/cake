/*
  C2Y 6.8.5.1 selection-header (N3580):

    expression
    declaration expression
    declaration-condition   (T D = X  is treated as  T D = X; D)

  Valid forms for both if and switch. The declared object is visible in the
  secondary block and in the else, and goes out of scope after the statement.
*/

struct S { int a; };

int f(void);
void g(int);

int main(void)
{
    if (int x = f()) { g(x); } else { g(x); }

    if (int x = f(); x > 0) { g(x); }

    if (int* p = 0) { g(1); } else { g(2); }

    if (register int r = 1) { g(r); }

    if (constexpr int k = 1) { g(k); }

    if ([[maybe_unused]] int a = 1) { }

    if (struct S s = {1}; s.a) { g(s.a); }

    switch (int c = f())
    {
        case 1: g(c); break;
        default: break;
    }

    switch (int c = f(); c + 1)
    {
        case 1: g(c); break;
        default: break;
    }

    /* x from the if above is out of scope here: no redeclaration */
    int x = 2;
    g(x);

    return 0;
}
