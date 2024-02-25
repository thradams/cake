struct X { int i; void* p; };
void* _Owner malloc(int sz);
int main() 
{
    struct X* _Owner p = malloc(1);
    static_state(p, "maybe-null");
    static_state(p->i, "uninitialized");
    static_state(p->p, "uninitialized");
}


void dummy() {}
#pragma cake diagnostic check "-Wmissing-destructor"