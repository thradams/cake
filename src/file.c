static_assert(_is_const(const int));
static_assert(!_is_const(const int*));
static_assert(_is_const(int* const));
