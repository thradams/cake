const int i;
static_assert(_Generic(&i, const int * : 1 ));

const int * const p;
static_assert(_Generic(&p, const int *  const * : 1 ));
