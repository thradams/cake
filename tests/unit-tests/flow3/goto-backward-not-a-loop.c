#pragma safety enable

/*
   Known issue: a backward goto (label appears before the goto that
   targets it) is not analysed as a loop at all.

   Unlike while/for, which run a two-pass analysis specifically to
   approximate the effect of repeated execution, a label is visited
   only once, in plain top-to-bottom order. When the goto below jumps
   back to L, flow3 registers a label map for L parented to the state
   at the goto site -- but nothing ever revisits the label's body
   using that map, since the label's statements were already analysed
   earlier in the single top-to-bottom pass.

   Expected (if goto-as-loop were handled soundly): a should be seen
   as an unbounded/ANY value (the loop can run any number of times), so
   compile_assert(a != 0) would be the only thing provable here.

   Actual (documents today's known-incomplete behaviour, not
   correctness): a is seen as exactly 1, as if the loop body executed
   exactly once and the backward jump had no effect on the analysis at
   all. The compile_assert(a == 1) below records this current gap on
   purpose -- if backward-goto loop convergence is ever implemented,
   this assertion should start FAILING, which is the signal to update
   this file and its documentation.
*/

void f(int n)
{
    int a = 0;
L:
    a = a + 1;
    if (n > 0)
    {
        n = n - 1;
        goto L;
    }
    /* expected today: a == 1 only (see "Actual" above -- this is the
       documented gap, not the sound answer) */
    // static_debug(a);
    compile_assert(a == 1);
}
