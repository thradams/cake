#pragma safety enable

/*
   Regression test: reassigning an _Owner pointer to null inside a
   catch block, after releasing the resource it held, must not make a
   later `return p;` warn that the pointee's lifetime has ended.

     int* _Owner _Opt p = malloc(sizeof *p);
     try
     {
         if (c)
             throw;
     }
     catch {
         free(p);      // releases the resource
         p = nullptr;  // p is now a fresh, non-owning null value
     }
     return p; // ok: p is EITHER still the live malloc'd pointer
               //     (try path, never freed) OR null (catch path,
               //     freed and reset) -- never a dangling pointer to
               //     a freed object.

   Root cause: after flow3_visit_try_statement merges the try arm
   (p still points at the live allocation X) and the catch arm (p was
   reset to null after freeing X) back into the enclosing map, X's own
   tracked entry ends up with TWO alternatives: "still alive" (from the
   try arm, where free() was never called) and "lifetime ended" (from
   the catch arm, where free() was called). flow3_check_object_access
   used to treat those as one flat, unconditional set: checking `*p`
   found X's "ended" alternative and warned "object '(*p)' lifetime has
   ended", regardless of the fact that the specific alternative of `p`
   being dereferenced right then (the try arm's "p points at X") is only
   ever paired with X's "still alive" alternative -- the "ended" one
   only ever coexists with p's OTHER alternative, "p is null" (which
   isn't a pointer to X at all, and is never dereferenced as one).

   Fixed by correlating: when checking the pointee of one specific
   alternative of a pointer, only that pointee's own alternatives whose
   origin is an ancestor of (i.e. was established on the same branch
   that produced) the pointer alternative being checked are considered.
*/

void* _Owner _Opt _Uninitialized malloc(unsigned long size);
void free(void* _Owner _Opt p);

int* _Owner _Opt f(int c)
{
    int* _Owner _Opt p = malloc(sizeof * p);
    try
    {
        if (c)
            throw;
    }
    catch {
        free(p);
        p = nullptr;
    }
    return p; /* ok: never a dangling pointer to a freed object */
}
