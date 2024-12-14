const int i = 0;
static_assert(_Generic(&i, const int *: 1));

const int *const p = 0;
static_assert(_Generic(&p, const int *const *: 1));
