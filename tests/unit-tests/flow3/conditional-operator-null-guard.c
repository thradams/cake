#pragma safety enable

/*
   `cond ? nullable : ""` -- the standard "default when absent" idiom.

   FIXED. Per-arm the value cannot be null: the true arm is only taken when the
   guard proved the pointer non-null, and the false arm is a string literal.

   The cause was not a type check and not a missing narrowing -- a probe inside
   the true arm showed exactly ONE alternative there, correctly narrowed. It was
   WHEN the value got resolved. The true arm stores a REF to the variable, and a
   REF is resolved lazily at the point of use, which is AFTER the merge, where
   the variable is back to both its null and non-null arms. The narrowing was
   discarded at resolution time rather than at capture time.

   Fixed in flow3.c's EXPR_CONDITIONAL result collection: each arm's REF
   alternatives are now resolved inside THAT ARM's map, binding the value to the
   state the arm actually had.

   Found in cake's own tokenizer.c:1611:

       struct stream stream =
       {
           .col = 1,
           .line = 1,
           .source = text,
           .current = text,
           .path = filename_opt ? filename_opt : ""   // warns
       };

   where filename_opt is `const char* _Opt`.

   Note the diagnostic used to name the expression '(null)' --
   flow3_expression_to_string has no name for a conditional operator. That is a
   separate, smaller defect: harmless now that the warning is gone, but it would
   make any future conditional-operator diagnostic unreadable.

   The false positive also PROPAGATED: the member and the local below inherited
   the unproven-null state, so it reappeared at every later use of a value that
   had been made non-null precisely to be safe. Five warnings came from three
   uses of the idiom.
*/

struct S
{
    const char* path;  /* non-_Opt: must never be null */
    int col;
};

void g(const char* s);

/* Designated initializer -- the tokenizer.c shape. */
void in_initializer(const char* _Opt filename_opt)
{
    struct S s = { .col = 1, .path = filename_opt ? filename_opt : "" };
    g(s.path);
}

/* Directly as an argument. */
void in_argument(const char* _Opt filename_opt)
{
    g(filename_opt ? filename_opt : "");
}

/* Through a local. */
void in_assignment(const char* _Opt filename_opt)
{
    const char* p = filename_opt ? filename_opt : "";
    g(p);
}

/*
   Control 1: the arm does not re-read the condition variable, so it never held
   a REF and was clean even before the fix.
*/
void arm_does_not_reread_ok(const char* _Opt filename_opt)
{
    const char* p = filename_opt ? "x" : "";
    g(p);
}

/*
   Control 2: the explicit if/else form, where the use happens INSIDE the arm
   and so never outlives the narrowing. Clean before and after.

   The two controls are what localised the bug: narrowing worked (control 2) and
   the merge was sound (control 1), which left only the case where a REF escapes
   its arm and is resolved after the merge.
*/
void spelled_out_ok(const char* _Opt filename_opt)
{
    const char* p;
    if (filename_opt)
        p = filename_opt;
    else
        p = "";

    g(p);
}
