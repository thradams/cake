typedef char* A;
typedef const A* B; 
static_assert(_is_same(typeof(B), char * const *));
