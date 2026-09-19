#pragma safety enable

/* assigning a local _Owner into an _Owner member is a move, using the local again warns; through a plain pointer parameter the overwrite also warns 26 (prior value unknown) */

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

/* ok: p moves into x.data of a local struct, destroyed once through the member */
void ok_move_into_member(void)
{
    struct Y* _Owner _Opt p = malloc_y();
    struct X x = { 0 };
    x.data = p; /* move */
    x_destroy(&x);
}

/* must warn: p was moved into x.data, free_y(p) is use-after-move */
void bad_use_after_move(void)
{
    struct Y* _Owner _Opt p = malloc_y();
    struct X x = { 0 };
    x.data = p; /* move */
    free_y(p); //lint 32 object 'p' is moved (see line 59)
    x_destroy(&x);
}

/* must warn 26: x is a pointer parameter, x->data's prior value is unknown and not freed first */
void move_into_member_via_param(struct X* x)
{
    struct Y* _Owner _Opt p = malloc_y();
    x->data = p; /* move */ //lint 26 assignment discards _Owner '.data' without releasing it first
}

/* must warn twice: the discard warning plus a real use-after-move on p */
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
