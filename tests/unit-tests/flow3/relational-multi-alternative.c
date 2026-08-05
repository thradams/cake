#pragma safety enable

/*
   Relational folding must consider ALL of an operand's alternatives, not
   just assume a single one. After `if (cond) a = 5; else a = 7;` the join
   leaves `a` with two alternatives {5, 7}. A relational comparison folds to
   a definite result only when every (left value, right value) pair agrees:

     a > 3   -> true  for both 5 and 7      (provable)
     a < 10  -> true  for both              (provable)
     a >= 5  -> true  for both              (provable)
     a > 6   -> 5 fails                      (NOT provable -- left unknown)
     a < 6   -> 7 fails                      (NOT provable)

   The evaluator follows the flow3_evaluate_binary_arithmetic iteration
   (per-alternative REF resolution, no size==1 shortcut), so this must
   compile with 0 errors / 0 warnings.
*/

void provable(int cond)
{
    int a;
    if (cond) a = 5; else a = 7;

    compile_assert(a > 3);
    compile_assert(a < 10);
    compile_assert(a >= 5);
    compile_assert(a <= 7);

    /* Deliberately NOT provable (documented; would error if enabled):
         compile_assert(a > 6);   // 5 fails
         compile_assert(a < 6);   // 7 fails
    */
}

/* Three-way join: a in {2, 4, 8}. */
void provable_three_way(int k)
{
    int a;
    if (k == 0) a = 2;
    else if (k == 1) a = 4;
    else a = 8;

    compile_assert(a > 1);   /* 2,4,8 all > 1 */
    compile_assert(a < 9);   /* all < 9 */
}
