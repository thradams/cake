
void* _Owner malloc(int size);

struct X {
    char* _Owner name;
};

void* _Owner f1() {
    struct X* _Owner p = malloc(sizeof(struct X));
    p->name = malloc(1);
    return p;
}

void dummy() {}
#pragma cake diagnostic check "-Wmissing-destructor"