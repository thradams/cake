#pragma safety enable

/* 85 is off by default and the suite passes -wd85; turn it back on here,
   because this false positive is only observable through it. */
#pragma cake diagnostic warning 85

/*
   A `break` taken from a branch the FIRST pass folded away still contributes
   its value to the state after the loop.

       for (;;)
       {
           if (it == 0) break;      <- pass 1 knows it != NULL, so this
           ...                            branch map is created dead
           it = it->parent;            <- ...but this can make it null, and
       }                                  then the break really is taken
       if (it != 0)                 <- so this is NOT always true

   flow3 got the state right (a probe after the loop shows a null alternative
   among the others), but the predicate behind warning 85 skipped alternatives
   whose origin map was marked unreachable -- and this null one's origin is
   exactly such a map. Only the non-null alternatives were left, so the test
   folded to true and everything after it read as unreachable.

   The loop widening and the &&/|| seeding still skip dead-origin
   alternatives; flow_object_truth deliberately does not.

   Reduced from src/parser.c find_next_subobject_old.
*/

struct object
{
    struct object* _Opt next;
    struct object* _Opt parent;
};

struct object* _Opt next_subobject(struct object* _Opt it)
{
    if (it == 0)
        return 0;

    for (;;)
    {
        if (it == 0)
            break;

        struct object* _Opt next = it->next;
        if (next != 0)
        {
            it = next;
            break;
        }

        it = it->parent;
    }

    if (it != 0)
    {
        return it;
    }

    /* reported "unreachable code" while the null alternative was skipped */
    int reached = 1;
    (void)reached;
    return 0;
}
