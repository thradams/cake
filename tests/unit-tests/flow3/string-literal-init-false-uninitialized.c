#pragma safety enable

/*
   Regression test for a false positive: a char* initialized directly
   from a string literal was reported as "passing a possible
   uninitialized object" when later passed to a function.

     char* n = "a";
     f(n);   // warned: passing a possible uninitialized object 'n'

   A string literal is never uninitialized -- it is a compile-time
   constant with static storage duration. flow3 must recognize the
   initializer's value and mark 'n' as initialized (and non-null).
*/

void f(char* n);

int main()
{
    char* n = "a";
    f(n); /* ok: n was initialized from a string literal, must NOT warn */
}
