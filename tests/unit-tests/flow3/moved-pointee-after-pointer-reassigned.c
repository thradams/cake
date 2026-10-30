#pragma safety enable

/* FIXED with catch-merges-path-before-owner-move.c: `e = tail(e)` then catch { e_delete(e) } is clean, each throw joins the catch under its own origin (expressions.c postfix_expression) */

struct T { int line; };
struct E { struct T* _Nonnull last_token; int x; };

void e_delete(struct E* _Owner _Nullable p);
struct E* _Owner tail(struct E* _Owner p);
struct E* _Owner make_a(void);
struct T* prev_token(void);

struct E* _Owner f(int cond)
{
    struct E* _Owner e = 0;
    try
    {
        e = make_a();
        if (e == 0) throw;

        struct T* pt = prev_token();
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
