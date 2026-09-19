#pragma safety enable

/* 85 is off by default and the suite passes -wd85; this false positive is only visible through it */
#pragma cake diagnostic warning 85

/* a break taken from a branch the first pass folded away still contributes its value after the loop: `if (it != 0)` is not always true (parser.c find_next_subobject_old) */

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
