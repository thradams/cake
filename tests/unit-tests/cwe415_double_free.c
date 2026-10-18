/* CWE415 Juliet CWE415_Double_Free__malloc_free_int_01 */

#pragma safety enable

_Uninitialized void* _Owner _Opt malloc(unsigned long n);
void free(void* _Owner _Opt p);
[[noreturn]] void exit(int status);

void bad()
{
    int* _Owner _Opt p = malloc(sizeof(int));
    if (p == nullptr) { exit(-1); }
    free(p);
    free(p); //lint 32
}
