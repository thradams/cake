void (*F)();
static_assert(_is_pointer(F));
static_assert(_is_integral(1));
int a[2];
static_assert(_is_array(a));
int((a2))[10];
static_assert(_is_array(a2));