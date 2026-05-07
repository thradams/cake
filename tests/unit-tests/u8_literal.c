
//C23 u8 character prefix
//https://open-std.org/JTC1/SC22/WG14/www/docs/n2418.pdf

//error: character not encodable in a single code unit.



//error: Unicode character literals may not contain multiple characters.

static_assert(u8'ab'); //lint 1370

//error: character not encodable in a single code unit.

static_assert(u8'¡' != 0); //lint 1360


static_assert(u8'~' == '~');
static_assert(_Generic(typeof(u8'~'), unsigned char : 1, default: 0));

int main()
{
    unsigned char c = u8'~';
}
