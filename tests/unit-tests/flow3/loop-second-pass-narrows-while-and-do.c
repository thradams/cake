#pragma safety enable

/*
   Same bug as loop-second-pass-narrows-first-iteration.c, but for `while` and
   `do`-`while`: the second (diagnostic) pass starts from the first pass's exit
   state, so a variable the body assigns still holds its post-iteration value.
   The controlling condition then folds statically and the branch it guards is
   reported unreachable -- even though the first iteration reaches it.

   Was:

     with_for        clean       (fixed)
     with_while      warning 68  on `flag = 1;`
     with_do_while   warning 68  on `flag = 1;`

   All three are clean now.

   FIXED -- by exactly the split the last paragraph of this comment called
   for. Porting the for-loop fix verbatim (body in its own map, widen
   body-moved counters, then join the extra-iteration arm) does make these two
   clean, but is UNSOUND for loops that release owners and regressed the
   codebase -- e.g. hashmap.c map_entry_delete:

       while (p != NULL) {
           struct map_entry* _Owner _Opt next = p->next;
           ...
           free(p);
           p = next;
       }

   Joining the zero-iterations state back in mixes "before free(p)" and
   "after free(p)" facts about the same pointee, so every p->data.* read
   picks up an alternative whose object was already freed (warnings 30/32).
   Counter loops are unaffected, which is why `for` was safe -- the
   free-and-advance idiom lives almost entirely in while loops.

   A correct fix has to join only the numeric/narrowing state and leave
   owner/lifetime facts out of the joined arm.

   That is what flow_join_first_iteration_values does: it unions the pre-loop
   value with the after-one-iteration value PER KEY, and skips any key whose
   alternatives carry an ownership or lifetime fact (imaginary != NONE), so
   the free-and-advance idiom keeps pass 1's state untouched. `do`-`while`
   gets the blanket join instead, which is sound there because the body always
   runs at least once.
*/

void with_for(int n, int flag)
{
    for (int k = 0; k < n; k++)
    {
        if (flag == 0)
        {
            flag = 1;
        }
    }
}

void with_while(int n, int flag)
{
    int k = 0;
    while (k < n)
    {
        if (flag == 0)
        {
            flag = 1; /* reachable on the first iteration when flag == 0 */
        }
        k++;
    }
}

void with_do_while(int n, int flag)
{
    int k = 0;
    do
    {
        if (flag == 0)
        {
            flag = 1; /* reachable on the first iteration when flag == 0 */
        }
        k++;
    } while (k < n);
}
