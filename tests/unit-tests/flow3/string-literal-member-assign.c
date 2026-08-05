#pragma safety enable

/*
   Function arguments are evaluated BEFORE the callee runs, so a mutable-pointer
   argument's effect on an object must be applied only AFTER the call -- not
   while the other arguments are still being read.

   Here `x.text = "a";` makes the member non-null, and `f(&x, x.text)` reads
   x.text at that current (non-null) value; f may modify *x, but only once it
   runs -- which is after both arguments are evaluated. So the call is clean.

   flow3 used to apply the "&x lets f modify *x" invalidation while checking the
   &x argument, which set x.text back to possibly-null before the x.text
   argument was read -- a false positive. Now those pointee write-effects are
   deferred until all arguments are evaluated.
*/

struct X
{
    char* _Opt text;
};

void f(struct X* p, char* s);

int main(void)
{
    struct X x = { 0 };
    x.text = "a";        /* string literal -> x.text is non-null */
    f(&x, x.text);       /* clean: x.text read before f can modify *x */
}
