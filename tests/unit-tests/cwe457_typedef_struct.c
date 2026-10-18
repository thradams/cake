/* CWE457 Juliet CWE457_Use_of_Uninitialized_Variable__struct_01 */

#pragma safety enable

void use(int i);

struct s { int a; };
typedef struct s S;
void bad()
{
    S v;
    use(v.a); //lint 30
}
