void* /*_Owner*/ f();
void free(void* /*_Owner*/ p);
int main() {
    void* /*_Owner*/ p = f();
    if (p)
    {
        free(p);
        p = f();
    }
}
#pragma cake diagnostic check "-Wmissing-destructor"
