/* with bool as fixed type, 0 and 1 are valid */
enum E : bool { A, B };
static_assert(A == 0);
static_assert(B == 1);
static_assert(sizeof(enum E) == sizeof(bool));
