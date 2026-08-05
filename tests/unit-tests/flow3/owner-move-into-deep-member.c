#pragma safety enable

/*
   Regression: moving an owner into a DEEP member destination reached through
   several `->` hops must record the move on the source, even when the
   destination's pointee object was not synthesized.

       free(p->b->name_opt->lexeme);
       char* _Opt _Owner temp = strdup(s);
       p->b->name_opt->lexeme = temp;     // <- temp is moved here

   The bug (seen on codegen.c:rename_file_scope_declarator_if_necessary,
   MSVC build only): if the synthetic pointee for `*(p->b->name_opt)` was
   not materialized (the arena/order for the chain didn't reach it), the
   destination `...->lexeme` had no tracked object, flow3_check_assigment
   returned early, and the normal move-marking never ran -- so `temp` was
   wrongly reported "owner object (temp) not moved" at scope exit.

   Fix: EXPR_ASSIGNMENT_ASSIGN records the source owner's move whenever the
   destination is an owner-typed lvalue, independent of whether the
   destination pointee resolved. This file must therefore report NO owner
   diagnostics (0 warnings). The move happens on every path that reaches
   the assignment; the `throw` on the null branch means no leak there.

   NOTE: this reproduces only under object layouts where the deep pointee
   isn't synthesized (it did not fire under gcc during development), so this
   sample documents the intended behavior rather than guaranteeing a trigger
   on every target.
*/

#define NULL ((void*)0)

void free(void* _Owner _Opt p);
char* _Owner _Opt strdup(const char* s);

struct name { char* _Owner _Opt lexeme; };
struct decl { struct name* _Opt name_opt; };
struct outer { struct decl* _Opt d; };

int move_into_deep_member(struct outer* p, const char* s)
{
    try
    {
        if (p->d == NULL) throw;
        if (p->d->name_opt == NULL) throw;

        free(p->d->name_opt->lexeme);
        char* _Opt _Owner temp = strdup(s);
        if (temp == NULL) throw;
        p->d->name_opt->lexeme = temp;   /* temp moved into the deep member */

        return 1;
    }
    catch
    {
    }
    return 0;
}

/* Direct (single-hop) owner move -- the always-worked baseline, kept as a
   contrast so both shapes are exercised.

   The early return on strdup failure used to leave n->lexeme freed and
   never reassigned -- exactly the borrowed-parameter contract violation
   flow3_check_non_dtor_param_owner_not_consumed_at_exit now catches
   (warning 72): n is a plain (non-_Dtor) parameter, so the caller still
   expects to own n->lexeme after this function returns. Restoring some
   value (even the same freed pointer's replacement attempt failing is not
   an option -- there is nothing to put back) means the failure path must
   instead leave n in a well-defined, still-valid state: null it out, since
   n->lexeme is _Opt. */
int move_into_shallow_member(struct name* n, const char* s)
{
    free(n->lexeme);
    n->lexeme = 0;
    char* _Opt _Owner temp = strdup(s);
    if (temp == NULL) return 0;
    n->lexeme = temp;
    return 1;
}
