typedef long l;
enum A : typeof(0L) { B };
enum B : l          { C };

_Static_assert(alignof(enum A) == 8, "");
_Static_assert(alignof(enum B) == 8, "");