#pragma safety enable

/* `catch { free(p); p = nullptr; } return p;` must not report an ended pointee: only pointee alternatives from the same branch as the pointer alternative are considered */

_Uninitialized void* _Owner malloc(unsigned long size);
void free(void* _Owner _Opt p);

int* _Owner f(int c)
{
    int* _Owner p = malloc(sizeof * p);
    try
    {
        if (c)
            throw;
    }
    catch {
        free(p);
        p = nullptr;
    }
    return p; /* ok: never a dangling pointer to a freed object */
}
