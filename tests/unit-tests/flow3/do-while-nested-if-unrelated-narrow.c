#pragma safety enable
#define NULL ((void*)0)

/* narrowing an unrelated `int opened` inside a nested if must not degrade ptk's non-null value (the old correlation filter in flow_narrow_map_into was removed) */

struct token
{
    struct token* _Owner _Opt next;
    int flags;
    int type;
};

void f(struct token* p);

void use(struct token* first_token, struct token* last_token)
{
    int opened = 0;
    struct token* _Opt ptk = first_token;
    do
    {
        if (ptk == NULL)
            break;

        if (ptk->flags && ptk->type == 2)
        {
            if (!opened)
            {
                opened = 1;
            }
            f(ptk); /* ok: ptk was proven non-null just above */
        }

        if (ptk == last_token)
            break;

        ptk = ptk->next;
    } while (ptk);
}

/* contrast: ptk can be null after the loop exits normally, still warns */
void use_after_loop(struct token* first_token, struct token* last_token)
{
    int opened = 0;
    struct token* _Opt ptk = first_token;
    do
    {
        if (ptk == NULL)
            break;

        if (ptk->flags && ptk->type == 2)
        {
            if (!opened)
            {
                opened = 1;
            }
            f(ptk); /* ok */
        }

        if (ptk == last_token)
            break;

        ptk = ptk->next;
    } while (ptk);

    f(ptk); //lint 35 passing a possible null pointer
}
