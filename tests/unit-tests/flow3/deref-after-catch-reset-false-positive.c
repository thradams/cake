#pragma safety enable

/*
   Regression test for a false positive found dogfooding flow3 on cake's own
   parser.c, where it accounted for 250 of that file's 256 warning-31s (14
   sites, each reported ~18 times over).

   Shape: a try/catch where the catch arm releases the object AND resets the
   pointer to null, followed by a null-guarded dereference:

       catch { init_declarator_delete(p); p = 0; }
       if (p && p->p_declarator) ...        // parser.c:3746 and 13 more

   The dereference is only reachable on the arm where the object was never
   released -- the catch arm's pointer alternative is null and is already
   skipped by the null check ahead of the lifetime check. But the POINTEE's
   tracked entry is shared: after the merge it holds the catch arm's
   FLOW3_IMAGINARY_ENDED alongside the try arm's live state, and the lifetime
   check scanned all of them. So an object that is alive on every path that can
   actually reach the `->` was reported as dead.

   Fix: apply the origin filter (flow3_map_is_ancestor_or_self) to the pointee's
   alternatives, keeping only those that can coexist with the pointer
   alternative being dereferenced. That is exactly what
   flow3_check_object_access already does for a pointee's own alternatives --
   see owner-reassigned-null-in-catch-false-positive.c, the same bug one level
   up, found in codegen.c. The `->` lifetime check simply never applied it.

   The same commit also added a report-once guard: a merged pointer can carry
   many alternatives aiming at the same pointee, and the identical diagnostic
   was emitted once per (pointer alternative x pointee alternative) pair.

   A second, independent instance of the same class of bug lived in
   flow3_check_object_init_assigment (the argument-passing / init path,
   see the `use(...)` call below) -- also fixed, by the same filter.
*/

void* _Owner _Opt _Clear calloc(unsigned long n, unsigned long s);

struct D { int x; };
struct I { struct D* _Owner _Opt p_declarator; };

void init_declarator_delete(struct I* _Owner _Opt p);
void use(struct D* p);
int maybe(void);

struct I* _Owner _Opt f(void)
{
    struct I* _Owner _Opt p = calloc(1, sizeof(struct I));
    try
    {
        if (p == 0) throw;
        if (maybe()) throw;
    }
    catch
    {
        /* releases the object and resets the pointer: the two facts belong
           together, and the null is what makes the guard below safe */
        init_declarator_delete(p);
        p = 0;
    }

    /* Must NOT warn "-> operator: pointed object lifetime has ended": the only
       arm that ended it is the one where p is null, which `p &&` excludes. */
    if (p && p->p_declarator)
    {
        /* Formerly a REMAINING GAP: flow3_check_object_init_assigment's own
           ENDED scan (the path `use(...)`'s argument-passing goes through)
           checked p_src_alternative->imaginary == FLOW3_IMAGINARY_ENDED with
           no origin filter at all, so the catch arm's ENDED fact reached it
           even though that arm's pointer is null and cannot get here. Fixed
           by applying the same flow3_map_is_ancestor_or_self(origin,
           ctx->p_current_flow3_map) filter used by flow3_check_object_access's
           already-fixed lifetime check. */
        use(p->p_declarator);
    }

    return p;
}
