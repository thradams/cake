typedef long l;
enum A : typeof(0L) { B };
enum B : l          { C };

_Static_assert(alignof(enum A) == alignof(long), "");
_Static_assert(alignof(enum B) == alignof(long), "");
