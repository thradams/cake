#pragma safety enable

void * _Opt _Owner _Clear malloc(unsigned long s);
void free(void * _Opt _Owner p);

int main() {
    void * _Owner _Opt p = malloc(1);
    free(p);
}
