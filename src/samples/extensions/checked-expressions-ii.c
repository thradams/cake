#pragma safety enable

void* _Owner malloc(unsigned long size);
void free(void* _Owner _Nullable ptr);

int main() {
    try
    {
        int * _Owner p = malloc(1)!;
        *p = 1;
        free(p);
    }
    catch {

    }
}
