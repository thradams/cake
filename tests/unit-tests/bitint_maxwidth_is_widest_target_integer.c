/* the widest _BitInt is the widest integer of the target: __int128 or long long */
static_assert(__BITINT_MAXWIDTH__ >= sizeof(long long) * 8);
unsigned _BitInt(__BITINT_MAXWIDTH__) a;
unsigned _BitInt(__BITINT_MAXWIDTH__ + 1) b; //lint 2060
