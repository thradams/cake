
//warning: array indirection
[[cake::w20]]
static_assert(_Generic(typeof(-*""), int : 1));

static_assert(_Generic(typeof(~false), int : 1));
static_assert(_Generic(typeof(~0ULL), unsigned long long: 1));

//warning: array indirection
[[cake::w20]]
static_assert(_Generic(typeof(~*""), int : 1));


static_assert(_Generic(typeof(~(unsigned char) 0), int : 1));
static_assert(_Generic(typeof(~(unsigned int) 0), unsigned int : 1));
