#pragma safety enable

/*
   Regression test for a false positive reported directly against real
   code shaped like this:

     switch (e) {
       case 1: ...; break;
       case 2: ...; break;
       default: break;
     }
     return 0;  // "unreachable code" -- false positive

   Root cause: `case N:` labels already created a FRESH child map
   (parented at the pre-switch state) to record the switch variable ==
   N, but `default:` took a shortcut and reused ctx->p_initial_map --
   literally the SAME map object captured before the switch statement
   even started -- directly as the current map, instead of wrapping it
   in a fresh child the way every `case` already did.

   If the default case's body ends in an unconditional jump (here,
   `break;`), the jump handling marks "the current map" dead so that
   syntactically-following dead code inside the same case is correctly
   detected. But since that "current map" was actually the pre-switch
   ancestor map itself, marking it dead corrupted that ancestor -- and
   the switch statement resumes execution on that very same map after
   merging, so the code that runs after the WHOLE switch inherited a
   stray is_dead=true and was misreported as unreachable.

   Fixed by giving `default:` a fresh child map, exactly like `case N:`
   already had.
*/

int use(int e)
{
    switch (e)
    {
        case 1:
            break;
        case 2:
            break;
        default:
            break;
    }
    return 0; /* ok: reachable, must NOT warn "unreachable code" */
}

/* Contrast: real dead code after `break;` inside the SAME case body is
   still correctly flagged. */
void real_unreachable(int e)
{
    switch (e)
    {
        default:
            break;
            int x = 1; //lint 68 unreachable code
            (void)x;
    }
}
