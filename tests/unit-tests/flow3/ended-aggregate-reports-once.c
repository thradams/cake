#pragma safety enable

/*
   A consumed aggregate must report ONCE, naming the object -- not once per
   member.

   flow3_object_leaves_in_state collapses a per-leaf report into a single one
   naming the aggregate. It used to require ALL leaves to be in the state, which
   is right for UNINITIALIZED and MOVED (a struct really can be half-filled, and
   a single owner member really can be moved out of one) but wrong for ENDED: a
   lifetime does not end member by member. The object is consumed as a whole and
   flow3_map_set_object_lifetime_ended stamps every leaf in one go.

   Requiring ALL meant one leaf without a map entry killed the collapse -- and a
   leaf that was never touched has no entry. The wider the struct, the likelier
   that is, so the collapse failed exactly where it mattered most. cake's own
   `struct expression` nests `struct type`, `struct object` and two lists, so
   some deep leaf always qualified: ONE consumed expression at expressions.c:2758
   produced 222 diagnostics, one per member.

   Worse than noise: the per-leaf form is unsuppressable. diagnostic() flushes
   the queue past 5 pending entries, so a `//lint` can never match more than a
   couple of them.

   ENDED now asks "any leaf", and the two other states keep "all".

   The struct below is deliberately wide and mixed -- owner pointers, plain
   pointers, a scalar, a nested aggregate -- so that some leaf goes untouched and
   the old all-leaves rule would decline to collapse.
*/

#define NULL ((void*)0)

struct inner
{
    int i;
    char* _Owner _Opt s;
};

struct wide
{
    struct wide* _Owner _Opt a;
    struct wide* _Owner _Opt b;
    struct wide* _Owner _Opt c;
    const char* _Opt name;
    int count;
    struct inner nested;
};

struct wide* _Owner _Opt make(void);
struct wide* _Owner _Opt tail(struct wide* _Owner p);
void del(struct wide* _Owner _Opt p);
int maybe(void);
void del_str(char* _Owner _Opt s);

/*
   The self-consuming-reassignment-through-a-join shape (see
   self-consuming-reassignment-in-try.c, still an open gap). What is pinned HERE
   is not that the warning fires -- it is that it fires exactly ONCE, by the
   object's name, rather than once per leaf of `struct wide`.
*/
void consumed_object_reports_once(void)
{
    struct wide* _Owner _Opt p = make();
    if (p == NULL)
        return;

    if (maybe())
        goto cleanup;

    p = tail(p);

    if (p == NULL)
        goto cleanup;

cleanup:
    del(p); //lint 31 one report for the object, not one per member
}

/*
   Control: MOVED keeps the all-leaves rule, so moving a SINGLE owner member out
   of an aggregate still names that member rather than claiming the whole struct
   moved. Clean here -- the point is that `s` is what moved, and reading the
   untouched members below is fine.
*/
void partial_move_names_the_member(struct inner* _Opt in)
{
    if (in == NULL)
        return;

    char* _Owner _Opt taken = in->s; /* only .s moves; .i is untouched */
    in->s = NULL;

    if (in->i > 0)
        in->i = 0;

    del_str(taken);
}
