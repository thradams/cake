/* CWE416 Juliet CWE416_Use_After_Free__malloc_free_int_01 */

#pragma safety enable

_Uninitialized void* _Owner _Opt malloc(unsigned long n);
void free(void* _Owner _Opt p);
[[noreturn]] void exit(int status);
void use(int i);

void bad()
{
    int* _Owner _Opt p = malloc(sizeof(int));
    if (p == nullptr) { exit(-1); }
    *p = 1;
    free(p);
    use(*p); //lint 31
}
