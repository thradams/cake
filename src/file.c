const int * const p;
_Static_assert(_Generic(&p, const int * const * : 1 ), "");