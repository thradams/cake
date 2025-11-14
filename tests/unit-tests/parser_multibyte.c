static_assert('d' == 100);
static_assert('dd' == 25700);
static_assert('ddd' == 6579300);
static_assert('dddd' == 1684300900);

static_assert("\300"[0] == -64);
static_assert('\300' == -64);
static_assert('a\300' == 25024);
