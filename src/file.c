/*
  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3214.pdf
*/

int main(void)
{
    const int * const p;
    static_assert(_Generic(p, const int * : 1 ));
    static_assert(_Generic(typeof(p), const int * const: 1));
  
    static_assert(_Generic(int, int : 1));    
    static_assert(_Generic(typeof("abc"), char [4]: 1));
}
