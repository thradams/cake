#pragma safety enable

/* `char* n = "a"; f(n);` must not report n uninitialized: a string literal is initialized and non-null */

void f(char* n);

int main()
{
    char* n = "a";
    f(n); /* ok: n was initialized from a string literal, must NOT warn */
}
