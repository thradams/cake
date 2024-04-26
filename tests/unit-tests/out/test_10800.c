#pragma safety enable


char* /*_Owner*/ strdup(const char* s);
void* /*_Owner*/ malloc(unsigned size);

void free(void* /*_Owner*/ /*_Opt*/ ptr);

struct X {
    char* /*_Owner*/ /*_Opt*/ name;
};

void x_destroy(struct X* /*_Obj_owner*/ p) {
    free(p->name);
}

void x_print(struct X* p)
{
    //printf("%s", p->name);
}

int main() {
    struct X x = { 0 };
    x.name = strdup("a");
    x_destroy(&x);    
    x_print(&x);
    #pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
}
#pragma cake diagnostic check "-Wmissing-destructor"

