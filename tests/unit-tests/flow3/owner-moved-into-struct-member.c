#pragma safety enable

/*
   Regression test: assigning a local _Owner object into an _Owner
   struct member -- reached either directly on a local struct or
   through a struct pointer parameter -- must be recognized as a MOVE.
   Ownership transfers from the local variable to the member, so using
   the local variable again afterward (e.g. freeing it, or moving it a
   second time) is a use-after-move / double-release bug and MUST warn.

   The two "via_param" functions below also demonstrate a related,
   separate warning: flow3_check_discarding_owner_before_overwrite
   fires on `x->data = p;` whenever `x` is a plain (non-_Opt) pointer
   PARAMETER, because x->data's INCOMING value (before this move) is
   unknown to flow3 -- it could already hold a live resource the caller
   forgot to release. This is a deliberate trade-off (see
   discard-owner-synthetic-seed-false-positive.c for the full
   rationale): warning here is a false positive when x->data really
   was already null/unowned, but staying silent would just as easily
   miss a genuine leak written the exact same way. Both are judged
   correct/expected in these samples.
*/

struct Y
{
    int value;
};

struct Y* _Owner _Opt malloc_y(void);
void free_y(struct Y* _Owner _Opt p);

struct X
{
    struct Y* _Owner _Opt data;
};

void x_destroy(_Dtor struct X* p)
{
    free_y(p->data);
}

/* ok: p's ownership moves into x.data; the struct is destroyed once,
   through the member -- must NOT warn. x is a local struct here (not a
   pointer parameter), so its members' incoming state is fully known to
   flow3 (zero-initialized above) -- no "unknown, possibly live" warning
   either. */
void ok_move_into_member(void)
{
    struct Y* _Owner _Opt p = malloc_y();
    struct X x = { 0 };
    x.data = p; /* move */
    x_destroy(&x);
}

/* must warn: p was already moved into x.data above, so `free_y(p)`
   here is operating on a moved-from (no longer owning) object. */
void bad_use_after_move(void)
{
    struct Y* _Owner _Opt p = malloc_y();
    struct X x = { 0 };
    x.data = p; /* move */
    free_y(p); //lint 32 object 'p' is moved (see line 59)
    x_destroy(&x);
}

/* must warn (discard, not use-after-move): x is a plain struct pointer
   PARAMETER here, so x->data's incoming value is unknown to flow3 --
   possibly already a live resource -- and this overwrite doesn't free
   it first. p itself is legitimately moved into x->data; the warning
   is purely about x->data's unknown PRIOR value, per the file comment
   above. */
void move_into_member_via_param(struct X* x)
{
    struct Y* _Owner _Opt p = malloc_y();
    x->data = p; /* move */ //lint 26 assignment discards _Owner '.data' without releasing it first
}

/* must warn TWICE: the same discard-of-unknown-prior-value warning as
   above, PLUS a genuine use-after-move on p. */
void bad_use_after_move_via_param(struct X* x)
{
    struct Y* _Owner _Opt p = malloc_y();
    x->data = p; /* move */ //lint 26 assignment discards _Owner '.data' without releasing it first
    free_y(p); //lint 32 object 'p' is moved (see line 82)
}

void use(void)
{
    struct X x = { 0 };
    move_into_member_via_param(&x);
    x_destroy(&x);
}
