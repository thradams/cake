#pragma safety enable

/*
   `if (d->m)` must narrow `d->m`, where `d` was assigned (not initialized)
   from a void*-returning allocator. FIXED.

   calloc returns `void*`, so the call site can only fabricate a pointee typed
   `void` -- and a void object has no members. Retyping it to the destination's
   pointee type is what flow3_apply_alloc_contract_to_dest exists for, but it
   was only called from the DECLARATOR-INITIALIZER path, so

       struct decl* _Owner _Opt d = calloc(1, sizeof *d);   // was fine

   worked while the split declare-then-assign form below did not. With the
   pointee left typed `void`, every `d->m` failed to resolve to a real member
   object and manufactured a fresh ANY alternative at each read -- so the guard
   narrowed one throwaway entry and the use inside the branch read another.

   The split form is not a corner case: it is how cake's own parser is written,
   because the object must exist before the `try` for the `catch` to release it.
   parser.c's declaration_core declares p_declaration NULL at the top, callocs
   it inside the try, and every later `p_declaration->member` false-warned --
   ids 33 and 35, including a `check_compiler_dianostic_suppression` call
   guarded by an `if` on the very line above it.

   Fixed by calling flow3_apply_alloc_contract_to_dest from the assignment path
   (flow3_check_assigment) as well, exactly as the initializer path does.

   The output parameter at line 18 matters: it leaves the member genuinely ANY,
   so the narrowing at line 20 is the only thing that can make line 22 safe. Had
   the member kept calloc's concrete zero it would have been provably null and
   the guard would have been decided statically instead.
*/

void* _Owner _Opt _Clear calloc(unsigned long n, unsigned long size);
void free(void* _Owner _Opt p);

struct token { int type; };
struct decl { struct token* _Opt lint_token; };

int set_it(struct token* _Opt* out);
void use(struct token* t);

/* The parser.c shape: declared null, assigned from calloc inside the body. */
void assigned_from_calloc(void)
{
    struct decl* _Owner _Opt d = 0;
    d = calloc(1, sizeof *d);
    if (d == 0) return;

    set_it(&d->lint_token);

    if (d->lint_token)
    {
        use(d->lint_token);
    }

    free(d);
}

/*
   Control: the initializer form, which already worked before the fix. Both must
   behave identically -- that equivalence is the point of the sample.
*/
void initialized_from_calloc_ok(void)
{
    struct decl* _Owner _Opt d = calloc(1, sizeof *d);
    if (d == 0) return;

    set_it(&d->lint_token);

    if (d->lint_token)
    {
        use(d->lint_token);
    }

    free(d);
}
