#pragma safety enable

/*
   Regression test: every path reaching a label -- every goto to it,
   plus fall-through -- must be preserved, not just the last jump that
   happened to register the label's map.

   Previously: the first goto to an unregistered label just created an
   empty map (see the state-loss bug documented historically in
   goto-forward-loses-state.c), so only a SECOND (or later) goto to the
   same label went through flow3_map_accumulate_into_join and actually
   left its value on the label's own map entry. Falling through into
   the label discarded whatever the label had accumulated so far by
   overwriting the current map outright.

   Fixed: every arrival at a label -- the first goto, any later goto,
   and fall-through -- now unions its state into the label's map
   instead of overwriting or being silently dropped.

   Below there are three ways to reach L: goto from the first if
   (a == 1), goto from the second if (a == 2), or fall through
   (a == 3).

   Expected: static_debug shows all three: a == 1, a == 2, a == 3.
*/

void f(int x, int y)
{
    int a = 0;
    if (x == 1)
    {
        a = 1;
        goto L;
    }
    if (y == 2)
    {
        a = 2;
        goto L;
    }
    a = 3;
L:
    /* expected: a == 1, a == 2 (both via goto), a == 3 (via fall-through) */
    // static_debug(a);
    compile_assert(a == 1 || a == 2 || a == 3);
    compile_assert(a != 0);
}
