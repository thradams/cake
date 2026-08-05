#pragma safety enable

/*
   Initialization from a NON-CONSTANT source (a parameter or another local).

   Here the object has no constant value, so the initial value comes from the
   init-EXPRESSION instead: flow3 copies whatever it currently knows about the
   source (its relation / alternatives) into the initialized variable. So the
   copy inherits any narrowing the source has, and stays branch-correlated --
   but a plain, unconstrained parameter carries nothing, so the copy is ANY.

   Use compile_assert (flow-checked), not static_assert (C11 compile-time).
*/

/* A narrowed parameter's relation carries into the copy. */
void carries_relation(int p)
{
    if (p > 0)
    {
        int x = p;                 /* x inherits p's ">0" */
        compile_assert(x > 0);
    }
}

/* An equality narrowing carries the concrete value across. */
void carries_value(int p)
{
    if (p == 5)
    {
        int x = p;
        compile_assert(x == 5);
    }
}

/* Initializing from a constant-per-branch parameter stays correlated. */
void chain_of_locals(int p)
{
    if (p == 7)
    {
        int a = p;                 /* a == 7 */
        int b = a;                 /* b inherits a == 7 */
        compile_assert(b == 7);
    }
}

/*
   An UNCONSTRAINED parameter carries no value, so the copy is unknown (ANY).
   These are NOT provable and are documented rather than asserted:

       void unknown(int p) {
           int x = p;
           compile_assert(x == p);   // ANY: no symbolic equality is tracked
       }

   NOTE: init copies the source's *known relations*, not a symbolic "x == p"
   link, so an unconstrained parameter stays unknown after the copy.
*/
