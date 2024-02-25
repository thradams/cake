
void* _Owner malloc(int i);
void free(void* _Owner p);

struct X {
    char* _Owner name;
};

void x_delete(struct X* _Owner p)
{
    if (p) {
        free(p->name);
        free(p);
    }
}

int main() {
    struct X* _Owner p = malloc(sizeof * p);
    x_delete(p);
}

void dummy()
{
} 

#pragma cake diagnostic check "-Wmaybe-uninitialized"

