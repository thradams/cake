#pragma safety enable

/*
   When the controlling expression of an if cannot narrow anything
   (e.g. a function call with unknown result), the true/false branch
   maps are the identity pair. Writes made inside the then-block must
   NOT leak unconditionally into the state after the if.

   Known issue: with the identity-pair convention, `a = 1` below is
   written directly into the pre-branch map, so the analyser believes
   a == 1 unconditionally after the if.
*/

int unknown();

void no_narrow_if()
{
    int a = 0;
    if (unknown())
    {
        a = 1;
    }
    // static_debug(a);
    /* Both alternatives must survive the join. */
    compile_assert(a == 0 || a == 1);
//    compile_assert(a != 2);


   // compile_assert(a == 1);
    
}

void no_narrow_if_with_else()
{
    int a = 0;
    if (unknown())
    {
        a = 1;
    }
    else
    {
        a = 2;
    }
    compile_assert(a == 1 || a == 2);

}
