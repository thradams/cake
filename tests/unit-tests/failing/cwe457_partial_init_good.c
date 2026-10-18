/* CWE457 Juliet CWE457_Use_of_Uninitialized_Variable__int_array_declare_partial_init_01 */

#pragma safety enable

void use(int i);

void good()
{
    int a[10];
    for (int i = 0; i < 10; i++) { a[i] = i; }
    for (int i = 0; i < 10; i++) { use(a[i]); }
}
