/* CWE401 Juliet CWE401_Memory_Leak__malloc_realloc_int_01 */

#pragma safety enable

_Uninitialized void* _Owner _Opt malloc(unsigned long n);
void* _Owner _Opt realloc(void* _Opt p, unsigned long n);
void free(void* _Owner _Opt p);
[[noreturn]] void exit(int status);

void bad()
{
    int* _Owner _Opt p = malloc(10 * sizeof(int));
    if (p == nullptr) { exit(-1); }
    p = realloc(p, 20 * sizeof(int)); //lint 26
    free(p);
}
