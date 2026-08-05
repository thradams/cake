#pragma safety enable

/*
   Regression test: a throw inside a catch block (a re-throw) must
   propagate to the ENCLOSING try, not recurse into the same
   try-statement's own (already fully populated, effectively closed)
   throw join.

   Previously: flow3_visit_try_statement set ctx->p_throw_join_map to
   this try's own join before visiting the try body, and only restored
   it to the enclosing value at the very end of the function -- after
   the catch body had already been visited too. So a throw executed
   inside the catch block incorrectly accumulated into the SAME try's
   throw join (the one meant only for throws out of the try body,
   which by then had already been fully collected and consumed to build
   the catch branch), instead of the throw join of whichever try
   actually encloses this one.

   Fixed: ctx->p_throw_join_map is restored to the enclosing value
   right after the try body is visited, before the catch body is
   visited at all -- so a re-throw in catch resolves against the
   correct (enclosing) context, exactly like any other statement in
   the catch body would.

   Below, the inner try's catch sets a = 3 and re-throws. That re-throw
   must reach the OUTER catch.

   Expected: the outer catch's static_debug shows a == 3.
*/

void f(int x)
{
    int a = 0;
    try
    {
        try
        {
            a = 1;
            if (x)
                throw;
            a = 2;
        }
        catch
        {
            a = 3;
            throw;
        }
    }
    catch
    {
        /* expected: a == 3 (the inner catch's value, via re-throw) */
        // static_debug(a);
        compile_assert(a == 3);
    }
}
