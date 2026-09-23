#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

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
