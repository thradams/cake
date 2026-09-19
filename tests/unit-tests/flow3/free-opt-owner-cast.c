#pragma safety enable

/* free takes `void* _Owner _Opt`, so passing a possibly-null _Owner member must not warn (object.c object_destroy) */

#define NULL ((void*)0)

void free(void* _Owner _Opt p);

struct object
{
    const char* _Opt _Owner member_designator;
};

void object_destroy(_Opt _Dtor struct object* p)
{
    free((void* _Owner _Opt)p->member_designator); /* ok: free accepts null */
}
