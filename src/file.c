static_assert(sizeof("Hello") == 6);
static_assert(sizeof(u8"Hello") == 6);
static_assert(sizeof(L"Hello") == (6 * sizeof(L' ')));
static_assert(sizeof(u"Hello") == (6 * sizeof(u' ')));
static_assert(sizeof(U"Hello") == (6 * sizeof(U' ')));
int main() {}