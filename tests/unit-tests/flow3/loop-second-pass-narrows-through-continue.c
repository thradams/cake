#pragma safety enable

/* FIXED: the diagnostics-only revisit of a body that always diverges restarts from the pre-loop state, so the continue edge's narrowing does not fold the first if (expressions.c expression_named_declarator) */

struct e
{
    int t;
    struct e* _Opt next;
};

int first_statement_wrongly_unreachable(const struct e* _Opt p)
{
    while (p != 0)
    {
        if (p->t == 1) /* reachable: the first iteration constrains nothing */
        {
            return 1;
        }

        if (p->t == 2)
        {
            continue;
        }

        break;
    }

    return 0;
}
