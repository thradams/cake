#pragma safety enable

/*
   Same shape as owner-resource-063.c: a forward goto skipping free(p) joins
   with a fall-through path that does call free(p). The merged state
   at the label must still show p as possibly-not-moved, so the leak
   is reported.
*/

void free(void* _Owner _Opt ptr);
void* _Owner _Opt _Uninitialized malloc(int size);

void f(int condition)
{
    int* _Owner _Opt p = malloc(sizeof(int)); // owner object (p) not moved (call-opt-nonnull see line 15)

    if (condition)
        goto end; /*memory leak o p*/

    free(p);
end:

} //lint 29 owner object (p) not moved (call-opt-nonnull see line 15)
