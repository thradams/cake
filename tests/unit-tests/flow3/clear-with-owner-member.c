#pragma safety enable

/*
   Regression tests: a struct holding an _Owner member, passed to a
   _Clear function. Clearing such a struct is not just "set the bits
   to zero" -- an owned resource must be RELEASED first, or the memory
   it points to leaks. Both halves are checked:

     1. flow3_check_clear_params_at_exit / flow3_check_clear_object_is_zero_at_exit
        (added earlier) verify every member -- including an _Owner
        pointer member -- is exactly 0 by the time the function exits.

     2. A new check was needed and added for THIS file: simply writing
        `p->text = 0;` over a live (non-null, non-moved) _Owner value
        satisfied check #1 (0 is 0) while still leaking whatever
        p->text used to point to. flow3 had no check anywhere for
        "overwriting a live _Owner without releasing it first" -- not
        for this case, not even for a plain local _Owner variable
        reassigned without freeing the old value. Added
        flow3_check_discarding_owner_before_overwrite, called from
        flow3_check_assigment before any assignment overwrites an
        _Owner destination (recursing into struct members, so this
        catches both `owner = new_value;` and `p->owner_member =
        new_value;` alike): warning 26 "assignment discards _Owner
        '...' without releasing it first".

   Together: a _Clear implementation for a struct with an _Owner
   member must both release it (or the discard warning fires) and
   leave it at exactly 0 (or the not-zero-at-exit warning fires).
*/

struct X
{
    char* _Opt _Owner text;
};

void free(void* _Opt _Owner p);

void clear_x_correct(_Clear struct X* p)
{
    /* release first, then zero -- both checks are satisfied */
    free(p->text);
    p->text = 0;
}

void clear_x_leaks(_Clear struct X* p)
{
    /* overwrites text with 0 directly -- satisfies "is zero at exit"
       but never released whatever text used to point to */
    p->text = 0; //lint 26 assignment discards _Owner '.text' without releasing it first (see line 46)
}

void clear_x_forgets_entirely(_Clear struct X* p)
{
    /* does nothing -- text keeps whatever non-zero state it entered
       the function with, so it's caught by the exit-is-zero check */
} //lint 69 _Clear parameter 'p' pointee (.text) is not zero at exit
