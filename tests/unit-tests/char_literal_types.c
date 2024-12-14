#define TYPE_IS(e, T) _Generic(typeof(e), T : 1, default: 0)

static_assert(U'ç' == 231);
static_assert(u'ç' == 231);
//static_assert('ç' == 231);

static_assert(TYPE_IS('a', int));
static_assert(TYPE_IS(u8'a', unsigned char));
static_assert(TYPE_IS(u'a', unsigned short));
static_assert(TYPE_IS(U'a', unsigned int));