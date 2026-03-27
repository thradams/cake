#pragma safety enable

void* _Owner _Opt calloc(unsigned long i, unsigned long size);
void free(void* _Owner _Opt ptr);

int main() {
    try {
       int * _Owner  p1 = calloc(1, sizeof * p1)!;       
       static_debug(p1);
       defer free(p1);
       throw;
    }
    catch { }   
}
