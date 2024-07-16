//en.cppreference.com/w/c/language/attributes/nodiscard.html
struct [[nodiscard]] error_info { int status; /*...*/ };
struct error_info enable_missile_safety_mode() { /*...*/ return (struct error_info){0}; }
void launch_missiles() { /*...*/ }
void test_missiles() {
   enable_missile_safety_mode(); // compiler may warn on discarding a nodiscard value
   launch_missiles();
}
struct error_info* foo() { static struct error_info e; /*...*/ return &e; }
void f1() {
    foo(); // nodiscard type itself is not returned, no warning
}
// nodiscard( string-literal ):
[[nodiscard("PURE FUN")]] int strategic_value(int x, int y) { return x ^ y; }
 
int main()
{
    strategic_value(4,2); // compiler may warn on discarding a nodiscard value
    int z = strategic_value(0,0); // OK: return value is not discarded
    return z;
}