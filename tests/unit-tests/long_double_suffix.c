
//make sure not octal
static_assert(_Generic(0.1L, long double: 1 , default : 0));
