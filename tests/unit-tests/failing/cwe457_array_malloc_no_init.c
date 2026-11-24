/* CWE457 Juliet CWE457_Use_of_Uninitialized_Variable__int_array_malloc_no_init_01 */

#pragma safety enable

_Uninitialized void* _Owner _Nullable malloc(unsigned long n);
void free(void* _Owner _Nullable p);
[[noreturn]] void exit(int status);
void use(int i);

void bad()
{
    int* _Owner _Nullable p = malloc(10 * sizeof(int));
    if (p == nullptr) { exit(-1); }
    use(p[0]);
    free(p);
}
