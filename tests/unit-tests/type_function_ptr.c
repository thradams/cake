int (*(*F1)(void))(int, int *);
int (*F2(void))(int, int *);
static_assert(_Generic(F1(), int (*)(int, int *): 1));
static_assert(_Generic(F2(), int (*)(int, int *): 1));
