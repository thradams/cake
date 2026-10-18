/* CWE590 Juliet CWE590_Free_Memory_Not_on_Heap__free_int_static_01 */

#pragma safety enable

void free(void* _Owner _Opt p);

void bad()
{
    static int a[10];
    int* p = a;
    free(p); //lint 78
}
