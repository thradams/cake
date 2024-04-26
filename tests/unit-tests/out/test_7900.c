#pragma safety enable


void* /*_Owner*/ /*_Opt*/ malloc(int i);
void free(void* /*_Owner*/ p);

struct X {
    char* /*_Owner*/ name;
};

int main() {
    struct X* /*_Owner*/ /*_Opt*/ p = malloc(sizeof * p);
    p = 0;
}

#pragma cake diagnostic check "-Wmissing-destructor"
