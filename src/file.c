//typeof_unqual(const int* const) p6;
static_assert(_is_same(typeof_unqual(const int* const), const int*));
