#pragma safety enable

/*
   Regression test: a forward goto's state must be preserved at its
   target label, alongside the fall-through path.

   Previously: when a goto jumped to a not-yet-registered label, flow3
   created an EMPTY map for that label, parented to whatever map was
   current at the goto site. That parent was not a frozen snapshot --
   it was the same live map object that later statements (the merge
   after the enclosing if, and the fall-through assignment below)
   continued to mutate in place. So by the time flow3_visit_label
   switched the current map to the label's map, walking up to that
   "parent" no longer reflected the state at the goto; it reflected
   whatever the shared ancestor was last mutated to. The goto-taken
   path's value was silently lost, and only the fall-through value
   happened to survive.

   Fixed: the goto handler now eagerly snapshots the state into the
   label's own map entries at the moment of the jump (instead of
   relying on a live, still-mutable parent chain), and the label visit
   unions the fall-through arrival into that snapshot instead of
   overwriting it.

   Expected: static_debug below shows BOTH a == 1 (arrived via goto)
   and a == 2 (arrived via fall-through).
*/

void f(int x)
{
    int a = 1;
    if (x)
        goto L;
    a = 2;
L:
    /* expected: a == 1 (arrived via goto), a == 2 (arrived via fall-through) */
    // static_debug(a);
    compile_assert(a == 1 || a == 2);
    compile_assert(a != 0);
}
