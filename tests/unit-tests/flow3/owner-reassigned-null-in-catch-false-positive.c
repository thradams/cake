#pragma safety enable

/* `catch { free(p); p = nullptr; } return p;` must not report an ended pointee: only pointee alternatives from the same branch as the pointer alternative are considered */

void* _Owner _Opt _Uninitialized malloc(unsigned long size);
void free(void* _Owner _Opt p);

int* _Owner _Opt f(int c)
{
    int* _Owner _Opt p = malloc(sizeof * p);
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
