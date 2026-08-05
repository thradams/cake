#pragma safety enable

/*
   Regression tests for pointer values that come into existence via
   array-to-pointer decay (as opposed to a normal scalar assignment).

   A decayed array address -- whether from a string literal or from a
   named array object -- is a compile-time fact: it is always non-null
   and always a fully-initialized pointer value, regardless of whether
   the array's *contents* are initialized. flow3 used to mishandle this
   in flow3_check_object_init_assigment: the source (the array/string
   literal) has per-element members while the destination (a scalar
   pointer) does not, so the member-wise copy loop did nothing and the
   destination silently kept its declaration-time UNINITIALIZED state --
   later misreported as "passing a possible uninitialized object" the
   first time the pointer was read.
*/

void take(char* p);

/* 1. String literal initializer. */
int from_string_literal(void)
{
    char* n = "a";
    take(n); /* ok: initialized from a string literal, non-null */
}

/* 2. Named array decaying to a pointer, then passed on. */
int from_named_array(void)
{
    char buf[8] = "hi";
    char* p = buf;
    take(p); /* ok: p is the (non-null) decayed address of buf */
}

/* 3. Reassignment (not just initialization) from a string literal. */
int reassigned_from_string_literal(int flag)
{
    char* n = "a";
    if (flag)
    {
        n = "b";
    }
    take(n); /* ok on both paths */
}
