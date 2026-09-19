#pragma safety enable

/* a pointer initialized from array-to-pointer decay (string literal or named array) is initialized and non-null regardless of the array's contents */

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
