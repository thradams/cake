void* _Owner malloc(int i);

struct X {
    void* _Owner text;
};

void x_change(struct X* list);

int main()
{
    struct X* _Owner x = malloc(sizeof * x);
    x_change(x);
#pragma cake diagnostic check "-Wmaybe-uninitialized"

}


void dummy() {}

#pragma cake diagnostic check "-Wmissing-destructor"

