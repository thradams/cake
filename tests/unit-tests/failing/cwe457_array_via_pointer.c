/* CWE457 Juliet CWE457_Use_of_Uninitialized_Variable__int_array_declare_no_init_01 */

#pragma safety enable

void use(int i);

void bad()
{
    int a[10];
    int* p = a;
    use(p[0]);
}
