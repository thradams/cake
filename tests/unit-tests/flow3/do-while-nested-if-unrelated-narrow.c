#pragma safety enable
#define NULL ((void*)0)

/*
   Regression test for a false positive reported directly against real
   code shaped like a token-scanning loop with a nested, textually
   unrelated if-statement inside a do-while body:

     do
     {
         if (ptk == NULL)
             break;

         if (ptk->flags && ptk->type == 2)
         {
             if (!opened)          // unrelated int, nothing to do with ptk
             {
                 opened = 1;
             }
             f(ptk); // false positive: "possible null pointer"
         }

         if (ptk == last_token)
             break;

         ptk = ptk->next;
     } while (ptk);

   Root cause: flow3_narrow_map_into (used whenever a bare variable like
   `opened` is narrowed by an `if`/`while` condition) used to run an
   extra "correlation filter" step after narrowing the condition
   variable itself: it walked EVERY tracked object across the entire
   ancestor map chain and, for any object whose alternatives' origins
   happened to be an ancestor-or-descendant of an origin that survived
   narrowing the condition variable, it would overwrite that unrelated
   object's entry directly in the freshly narrowed map with a filtered
   (and sometimes far less precise, even generic ANY) subset of its
   alternatives.

   This was meant to support a narrow, specific pattern:
     if (a) { b = 1; }
     if (a) { static_debug(b); }  // must see only b==1
   where two *separate* ifs test the *same* condition and are genuinely
   correlated through it. But "shares an ancestor map" is true of nearly
   every object tracked inside the same function -- it is not a real
   signal of correlation. Narrowing a completely unrelated `int opened`
   inside a nested `if (!opened)` ended up corrupting `ptk`'s own
   already-correct non-null alternatives with a stale, degraded ANY
   value pulled from an unrelated ancestor entry, purely because both
   objects happened to share some common ancestor map by coincidence of
   nesting depth.

   Fixed by removing the correlation filter entirely; no regression
   sample depended on the specific two-ifs-same-condition pattern it was
   meant to protect, and ordinary per-branch narrowing/merging already
   correctly preserves each object's own real value (see
   flow3_map_merge_arms's origin re-tagging).
*/

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

/* Contrast: ptk really can be null once the loop exits normally -- must
   still warn. Same shape as `use`, just with one more use of ptk after
   the loop. */
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
