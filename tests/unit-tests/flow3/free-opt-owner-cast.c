#pragma safety enable

/* free takes `void* _Owner _Nullable`, so passing a possibly-null _Owner member must not warn (object.c object_destroy) */

#define NULL ((void*)0)

void free(void* _Owner _Nullable p);

struct object
{
    const char* _Owner member_designator;
};

void object_destroy(_Dtor struct object* p)
{
    free((void* _Owner)p->member_designator); /* ok: free accepts null */
}
