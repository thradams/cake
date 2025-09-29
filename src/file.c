#define IS_SAME_TYPE(T1, T2) \
  _Generic(typeof(T1), typeof(T2): 1, default: 0)

int main()
{
    double const x = 78.9;
    double y = 78.9;
    auto q = x;
    static_assert(IS_SAME_TYPE(q, double));
    auto const p = &x;
    static_assert(IS_SAME_TYPE(p, const double* const));
    auto const r = &y;
    static_assert(IS_SAME_TYPE(r, double* const));
    auto s = "test";
    static_assert(IS_SAME_TYPE(s, char*));
}
