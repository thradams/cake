#pragma safety enable

/*
   Multiple alternatives across binary operators, with CORRELATED joins.

   After a control-flow join a variable can hold several possible values
   (alternatives), each tagged with the branch (origin) that produced it.
   Binary operators combine only operand alternatives that can coexist on the
   same path (matching origins), so a join stays correlated.

   Here, after the if/else, `a` is {1@then, 3@else} and `b` is
   {2@then, 4@else}. Because the analyzer pairs values by branch:

     a + b  ->  {3, 7}        (1+2 and 3+4 only -- NOT 1+4 or 3+2)
     a * b  ->  {2, 12}       (1*2 and 3*4)
     a - b  ->  {-1}          (1-2 and 3-4 are both -1)

   A comparison / compile_assert folds only when it holds for every
   resulting alternative.
*/

void arithmetic(int c)
{
    int a, b;
    if (c) { a = 1; b = 2; }
    else   { a = 3; b = 4; }

    /* a + b in {3, 7} -- correlation excludes 5 */
    compile_assert(a + b == 3 || a + b == 7);
    compile_assert(a + b != 5);
    compile_assert(a + b >= 3);
    compile_assert(a + b <= 7);

    /* a * b in {2, 12} */
    compile_assert(a * b == 2 || a * b == 12);
    compile_assert(a * b != 0);

    /* a - b is exactly {-1} (both branches give -1) */
    compile_assert(a - b == -1);

    /* Deliberately NOT provable:
         compile_assert(a + b == 3);   // 7 also possible
         compile_assert(a * b == 2);   // 12 also possible
    */
}

/* Different operand alternatives can collapse to a single result value:
   a is {2, 5}, b is {3}, and 2 % 3 == 5 % 3 == 2, so a % b is exactly {2}. */
void collapses_to_one(int c)
{
    int a, b;
    if (c) { a = 2; b = 3; }
    else   { a = 5; b = 3; }

    compile_assert(a % b == 2);
}

/* Three-way join feeding a comparison: x is {10, 20, 30}. */
void three_way(int k)
{
    int x;
    if (k == 0)      x = 10;
    else if (k == 1) x = 20;
    else             x = 30;

    compile_assert(x == 10 || x == 20 || x == 30);
    compile_assert(x >= 10);
    compile_assert(x <= 30);
    compile_assert(x != 0);
}
