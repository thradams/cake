#pragma safety enable

/* `(void*)p` of an owner is still that owner: passing it moves p */

void free(void* _Owner _Nullable p);

struct X { const char* _Owner name; };

/* no 78 at the call, no 26 at the assignment: name was moved */
void member_then_null(struct X* p)
{
    free((void*)p->name);
    p->name = 0;
}

/* no 29 at the end of the function: s was moved */
void const_local_owner(const char* _Owner s)
{
    free((void*)s);
}

void plain_owner(char* _Owner s)
{
    free((void*)s);
}

/* the move reached s itself */
void used_after_move(char* _Owner s)
{
    free((void*)s);
    free(s); //lint 32
}
