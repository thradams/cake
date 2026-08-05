#pragma safety enable
#define NULL ((void*)0)

/*
   Regression test for a false positive reported directly against real
   code shaped like the classic readdir() loop:

     struct dirent* _Opt dp;
     while ((dp = readdir(dir)) != NULL)
     {
         use(dp->d); // "dp" possibly null -- false positive
     }

   Root cause: EXPR_ASSIGNMENT_ASSIGN never seeded ITS OWN expression
   object at all -- it only ever updated p_expression->left->object
   (the destination sub-expression's own node), via
   flow3_check_assigment. That's enough for a plain assignment
   STATEMENT (`dp = readdir(dir);` followed by a separate
   `if (dp != NULL)`), since that later reads dp itself, not the
   assignment expression's result.

   But when the assignment is embedded as an OPERAND of something else
   -- `(dp = readdir(dir)) != NULL` -- the enclosing `!=` looks up
   THIS node's own object and found nothing there at all, so it
   couldn't narrow anything: dp still looked possibly-null inside the
   loop body, even though the very condition that let execution in had
   already proven it wasn't.

   Fixed by seeding the assignment expression's own object as a REF to
   the real, persistent destination object (resolving one REF hop, same
   as every other consumer of a REF alternative) -- so any later lookup
   on the assignment's own object (e.g. from an enclosing `==`/`!=`)
   transparently finds the destination's real, correctly narrowed
   alternatives.
*/

struct dirent { int d; };
struct DIR { int x; };
struct dirent* _Opt readdir(struct DIR* dir);

void use_while_loop(struct DIR* dir)
{
    struct dirent* _Opt dp;
    while ((dp = readdir(dir)) != NULL)
    {
        int x = dp->d; /* ok: dp is non-null here */
    }
}

/* Contrast: a plain assignment with no null-check at all is still
   correctly flagged. */
void use_no_check(struct DIR* dir)
{
    struct dirent* _Opt dp;
    dp = readdir(dir);
    int x = dp->d; //lint 33 -> operator applied to a null pointer
}

/* Contrast: after the loop above exits, dp really can be null (or
   uninitialized) -- still correctly flagged. */
void use_after_loop(struct DIR* dir)
{
    struct dirent* _Opt dp;
    while ((dp = readdir(dir)) != NULL)
    {
    }
    int x = dp->d; //lint 33 -> operator applied to a null pointer
}
