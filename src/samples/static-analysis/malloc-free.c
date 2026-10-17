#pragma safety enable

_Clear void * _Opt _Owner malloc(unsigned long s);
void free(void * _Opt _Owner p);

int main() {
    void * _Owner _Opt p = malloc(1);
    free(p);
}
