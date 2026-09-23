/*
  _BitInt(N) is lowered to the smallest standard integer that holds N bits.
  Widths up to 64 are supported.
*/
typedef unsigned _BitInt(24) u24;

struct S {
    _BitInt(12) m;
    unsigned _BitInt(3) n : 2;
};

static_assert(sizeof(_BitInt(12)) == 2);
static_assert(_Generic(1wb, _BitInt(2): 1, default: 2) == 1);

int main()
{
    _BitInt(12) a = 100;
    unsigned _BitInt(12) b = 4095uwb;
    signed _BitInt(40) c = -1;
    u24 d = 0xFFFFFF;

    constexpr unsigned _BitInt(12) w = (unsigned _BitInt(12)) 4096;
    static_assert(w == 0);

    return a + b + c + d;
}
