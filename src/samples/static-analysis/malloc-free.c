#pragma safety enable

_Clear void * _Owner malloc(unsigned long s);
void free(void * _Nullable _Owner p);

int main() {
    void * _Owner p = malloc(1);
    free(p);
}
