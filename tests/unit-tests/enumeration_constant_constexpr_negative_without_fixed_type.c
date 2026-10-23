/* the value comes from a constexpr object: the enum must be signed */
constexpr long long n = -5;
enum E { A = n, B = 1 };
static_assert(A < 0);
static_assert((enum E)-1 < 0);
