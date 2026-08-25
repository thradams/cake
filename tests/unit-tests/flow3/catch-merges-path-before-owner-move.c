#pragma safety enable

/*
   False "object '*p' lifetime has ended" in a catch, for an owner that was
   consumed and REASSIGNED inside the try.

       p = calloc(...);
       if (p == 0) throw;

       if (step() != 0) throw;   <- an exit BEFORE the move

       p = tail(p);              <- consumes the old object, returns a new one
       if (p == 0) throw;
       ...
       catch { expression_delete(p); }   <- reported use-after-end -- WRONG

   The catch is a join of every throw site. On the `step()` path, p still
   names the object calloc returned and it is perfectly alive. On the paths
   after `p = tail(p);`, p names something else entirely (a new object, or
   null) and the OLD object is the one whose lifetime ended. The report pairs
   the first path's pointer value with the second path's lifetime fact.

   Minimal trigger: the extra throw between the allocation and the move.
   Remove it -- leaving only the post-calloc and post-move throws -- and the
   file goes clean, because then no live-at-that-point path reaches the catch.
   No member write is needed (unlike
   moved-pointee-after-pointer-reassigned.c, which reduces the same defect
   from a shape that also required `e->last_token = pt;`).

   FIXED. The throw-join used to append each jump's state keeping the
   alternatives' original origins -- and origins are per-MAP, so inside one
   straight-line try block every fact shares one and the join could not tell
   "p named this object" (recorded at the first throw) from "that object
   ended" (recorded later, on a path that diverged). Each throw now
   contributes under its own snapshot map as origin, so facts from different
   jumps land on sibling maps, which flow_map_is_ancestor_or_self already
   treats as states that never coexist. Identical facts are deduplicated, so
   agreeing jumps do not multiply alternatives.

   Reduced from src/expressions.c postfix_expression_compound_func_literal,
   which is `//lint 31 31 BUG in flow` today; postfix_expression and
   unary_expression carry the same shape.
*/

struct expression { int kind; };

void* _Owner _Opt calloc(int n, unsigned long size);
void expression_delete(struct expression* _Owner _Opt p);
struct expression* _Owner _Opt tail(struct expression* _Owner p);
int step(void);

struct expression* _Owner _Opt compound_literal(void)
{
    struct expression* _Owner _Opt p = 0;
    try
    {
        p = calloc(1, sizeof * p);
        if (p == 0)
            throw;

        if (step() != 0)
            throw;

        p = tail(p);
        if (p == 0)
            throw;
    }
    catch
    {
        expression_delete(p);
        p = 0;
    }
    return p;
}
