#pragma safety enable

/* a cast changes the type only: `(void*)p` keeps p's _Owner and nullability */

char* _Owner make(void);
void take(void* _Owner p);

void owner_stays_owner(char* _Owner p)
{
    take((void*)p);
}

void owner_is_moved_through_cast(char* _Owner p)
{
    void* _Owner q = (void*)p;
    take(q);
}

void non_owner_stays_non_owner(char* p)
{
    void* _Owner q = (void*)p; //lint 78
    take(q);
}

void nullable_stays_nullable(void* _Nullable p)
{
    char* q = (char*)p;
    (void)*q; //lint 33
}

void nonnull_stays_nonnull(void* p)
{
    char* q = (char*)p;
    (void)*q;
}

void temporary_owner_is_lost(void)
{
    void* p = (void*)make(); //lint 79
    (void)p;
}
