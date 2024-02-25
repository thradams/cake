void* _Owner malloc(unsigned long size);
void free(void* _Owner ptr);

struct X {
    char* _Owner text;
};

void x_delete(struct X* _Owner  p);

int main() {
    struct X* _Owner p = malloc(sizeof(struct X));
    x_delete(p); /*uninitialized*/
}


void dummy()
{
} 

//flow analyze
#pragma cake diagnostic check "-Wmaybe-uninitialized"

