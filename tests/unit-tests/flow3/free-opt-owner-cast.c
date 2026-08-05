#pragma safety enable

/*
   Freeing a possibly-null _Owner member through an (_Owner _Opt) cast.

   free's parameter is `void* _Owner _Opt` -- it accepts null (free(NULL) is
   valid C). member_designator is `const char* _Opt _Owner`, so it is possibly
   null. Passing it to free must therefore NOT warn "passing a possible null
   pointer to non-nullable parameter" (warning 35).

   This is the reduced form of object.c:object_destroy. free is declared in
   ownership.h as `void free(void* _Owner _Opt ptr)`, so its parameter accepts
   null and this call is clean.
*/

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
