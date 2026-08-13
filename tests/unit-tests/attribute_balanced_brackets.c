/*
   brackets inside an attribute-argument-clause must be balanced correctly.
   the closing ']' and '}' were being compared against '[' and '{',
   making the decrement unreachable, so any '[' or '{' left the counter
   non zero and reported a false 'expected ']' before ')''.
*/

[[foo(a[1])]] void f1(void); //lint 9 warning 'foo' is not an standard attribute

[[foo(a{1})]] void f2(void); //lint 9 warning 'foo' is not an standard attribute

[[foo(a[1], b[2])]] void f3(void); //lint 9 warning 'foo' is not an standard attribute

int main(void)
{
    return 0;
}
