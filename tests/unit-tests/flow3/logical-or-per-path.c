#pragma safety enable

/*
   Per-path evaluation of `||` in compile_assert.

   `compile_assert(X || Y)` must hold on EVERY path, i.e. every possible value
   must satisfy X or Y. This requires the analyzer to keep each value tagged
   with the branch it came from (its origin) all the way through:

     1. arithmetic keeps the operand-pair's branch  (a+b = {3@then, 7@else})
     2. a comparison yields a per-path boolean       (a+b==7 = {0@then,1@else})
     3. || combines those booleans path-by-path

   After `if (c){a=1;b=2;}else{a=3;b=4;}`, a+b is {3, 7} (correlated).
*/

void must_pass(int c)
{
    int a, b;
    if (c) { a = 1; b = 2; }
    else   { a = 3; b = 4; }

    /* Every value of a+b (3 or 7) satisfies one side. */
    compile_assert(a + b == 3 || a + b == 7);
    compile_assert(a + b < 5 || a + b > 6);      /* 3<5, 7>6 */

    /* Three-way join, three-way OR. */
    int x;
    if (c) x = 10; else x = 20;
    compile_assert(x == 10 || x == 20);
}

/* These SHOULD fail -- some path satisfies neither side -- and are left
   commented so this file compiles clean. Enabling any one produces
   "compile_assert failed":

   void must_fail_a(int c) {
       int a,b; if(c){a=1;b=2;}else{a=3;b=4;}
       compile_assert(a + b == 4 || a + b == 7);   // a+b==3 satisfies neither
   }
   void must_fail_b(int c) {
       int a,b; if(c){a=1;b=2;}else{a=3;b=4;}
       compile_assert(a + b < 3 || a + b > 7);      // neither covers 3 or 7
   }
*/

#pragma safety enable
void f(int c){
    int a, b;
    if (c) { a = 1; b = 2; } else { a = 3; b = 4; }

    /* MUST pass (every path satisfies one side) */
    compile_assert(a + b == 3 || a + b == 7);
    compile_assert(a + b != 5 && a + b != 100);

// MUST fail (a+b==3 path satisfies neither) -- enable one to check:
       compile_assert(a + b == 4 || a + b == 7); //lint 67 a+b==3 path satisfies neither side
       compile_assert(a + b == 3 || a + b == 5); //lint 67 mixed-left
       compile_assert(a + b == 3 && a + b == 7); //lint 67 67 mixed-left AND (fails on both arms)
    
}
