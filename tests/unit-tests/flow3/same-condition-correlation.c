#pragma safety enable

/*
   Correlation across two SEPARATE branches guarded by the SAME predicate.

   Both `if`s test the identical predicate `c > 2`. On the path where the
   second one is entered, the first was also entered, so `p` was set to `&i`
   and cannot be null. flow3 now recognizes this: a comparison controlling more
   than one branch reuses the branch id first assigned to it, so a value guarded
   by that predicate stays correlated across both branches. The deref then
   drops `p`'s null alternative -- it belongs to the predicate's false side,
   which conflicts with being inside its true side.

   Soundness: the shared id is used ONLY while the predicate's operands are
   unchanged. Writing an operand invalidates the predicate, and a syntactically
   different predicate is never merged -- both keep warning (see below).
*/

int f(int c)
{
    int i = 0;
    int* _Opt p = 0;

    if (c > 2) p = &i;
    if (c > 2) i = *p;   /* clean: same predicate proves p is non-null here */

    return i;
}

/* Works through `->` as well. */
struct point { int x, y; };

int g(int c, struct point pt)
{
    struct point* _Opt p = 0;

    if (c > 2) p = &pt;
    if (c > 2) return p->x;   /* clean */

    return 0;
}

/*
   STILL WARN -- correctly not correlated (shown in comments so this file is
   clean):

   int reassigned(int c) {              // operand written between the ifs
       int i = 0; int* _Opt p = 0;
       if (c > 2) p = &i;
       c = other();                     // c changed -> predicate invalidated
       if (c > 2) i = *p;               // warns: c>2 no longer implies the first
       return i;
   }

   int different(int c) {               // a different predicate
       int i = 0; int* _Opt p = 0;
       if (c > 2) p = &i;
       if (c > 3) i = *p;               // warns: c>3 does not imply c>2
       return i;
   }
*/
