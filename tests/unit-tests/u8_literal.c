
//C23 u8 character prefix
//https://open-std.org/JTC1/SC22/WG14/www/docs/n2418.pdf

static_assert(u8'÷' == 0xF7);
#pragma cake diagnostic check "-E1360"

static_assert(u8'ab');
#pragma cake diagnostic check "-E1370"

static_assert(u8'¡' != 0);
#pragma cake diagnostic check "-E1360"


static_assert(u8'~' == '~');
static_assert(_Generic(typeof(u8'~'), unsigned char : 1 , default: 0));

int main(){
    unsigned char c = u8'~';
}
