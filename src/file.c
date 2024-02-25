void free(void* _Owner ptr);
void* _Owner malloc(int size);
char* _Owner strdup(const char*);

struct X {
    char* _Owner name;
};

struct Y {
    struct X x;
    struct X* px;
};

void x_destroy(struct X* _Obj_owner p)
{
    free(p->name);
}

void f(struct Y* p)
{
    x_destroy(p->px);
}

int main() {
    struct Y  y = {};
    struct* p = &y.x;
    x_destroy(&y.x);
}




//flow analyze
#pragma cake diagnostic check "-Wmust-use-address-of"

