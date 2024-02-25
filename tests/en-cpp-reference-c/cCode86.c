//en.cppreference.com/w/c/language/attributes.html
[[gnu::hot]] [[gnu::const]] [[nodiscard]]
int f(void); // declare f with three attributes
 
[[gnu::const, gnu::hot, nodiscard]]
int f(void); // the same as above, but uses a single attr
             // specifier that contains three attributes
 
int f(void) { return 0; }
 
int main(void)
{
}