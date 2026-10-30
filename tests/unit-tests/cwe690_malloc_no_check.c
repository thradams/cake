/* CWE690 Juliet CWE690_NULL_Deref_From_Return__int_malloc_01 */

#pragma safety enable

_Uninitialized void* _Owner malloc(unsigned long n);
void free(void* _Owner _Opt p);

void bad()
{
    int* _Owner p = malloc(sizeof(int));
    *p = 5; //lint 33
    free(p);
}
