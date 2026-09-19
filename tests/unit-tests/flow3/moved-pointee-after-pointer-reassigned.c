#pragma safety enable

/* FIXED with catch-merges-path-before-owner-move.c: `e = tail(e)` then catch { e_delete(e) } is clean, each throw joins the catch under its own origin (expressions.c postfix_expression) */

struct T { int line; };
struct E { struct T* last_token; int x; };

void e_delete(struct E* _Owner _Opt p);
struct E* _Owner _Opt tail(struct E* _Owner p);
struct E* _Owner _Opt make_a(void);
struct T* _Opt prev_token(void);

struct E* _Owner _Opt f(int cond)
{
    struct E* _Owner _Opt e = 0;
    try
    {
        e = make_a();
        if (e == 0) throw;

        struct T* _Opt pt = prev_token();
        if (pt == 0) throw;

        e->last_token = pt; /* remove this line and the warning disappears */

        e = tail(e);        /* moves the old pointee, e now points elsewhere */
        if (e == 0) throw;
    }
    catch
    {
        e_delete(e);
        e = 0;
    }
    return e;
}
