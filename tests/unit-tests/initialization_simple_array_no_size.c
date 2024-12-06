//C:\Users\thiago\source\repos\cake_private\tests\unit-tests\initialization\simple_array_no_size_initialization.c
constexpr int i[] = {5, 6};
static_assert(i[0] == 5);
static_assert(i[1] == 6);
