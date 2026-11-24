/* CWE416 Juliet CWE416_Use_After_Free__return_freed_ptr_01 */

#pragma safety enable

_Uninitialized void* _Owner _Nullable malloc(unsigned long n);
void free(void* _Owner _Nullable p);
[[noreturn]] void exit(int status);

char* _Nullable helper()
{
    char* _Owner _Nullable p = malloc(10);
    if (p == nullptr) { exit(-1); }
    p[0] = 0;
    char* _Nullable r = p;
    free(p);
    return r; //lint 31
}
