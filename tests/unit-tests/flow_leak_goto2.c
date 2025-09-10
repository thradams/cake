#pragma safety enable


void free(void* _Owner _Opt ptr);
void* _Owner _Opt malloc(int size);

void f(int condition)
{
    int* _Owner _Opt p [[cake::leak]] = malloc(sizeof(int));

    if (condition)
        goto end; /*memory leak o p*/

    free(p);
end:

}
