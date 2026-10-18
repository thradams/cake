/* CWE416 Juliet CWE416_Use_After_Free__return_freed_ptr_01 */

#pragma safety enable

_Uninitialized void* _Owner _Opt malloc(unsigned long n);
void free(void* _Owner _Opt p);
[[noreturn]] void exit(int status);

char* _Opt helper()
{
    char* _Owner _Opt p = malloc(10);
    if (p == nullptr) { exit(-1); }
    p[0] = 0;
    char* _Opt r = p;
    free(p);
    return r; //lint 31
}
