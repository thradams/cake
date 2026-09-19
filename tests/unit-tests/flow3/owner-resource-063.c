#pragma safety enable

/* a forward goto skipping free(p) joins a fall-through path that frees it: at the label p is possibly not moved, the leak is reported */

void free(void* _Owner _Opt ptr);
void* _Owner _Opt _Uninitialized malloc(int size);

void f(int condition)
{
    int* _Owner _Opt p = malloc(sizeof(int)); // owner object (p) not moved (call-opt-nonnull see line 17)

    if (condition)
    {
        goto end;
    }

    free(p);
end:
} //lint 29 owner object (p) not moved (call-opt-nonnull see line 17)
