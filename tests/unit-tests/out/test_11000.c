void* /*_Owner*/ malloc(unsigned long size);
void free(void* /*_Owner*/ ptr);

struct X {
    char* /*_Owner*/ name;
};

int main()
{
    struct X* p = (struct X* /*_Owner*/) malloc(1);
}

//flow analyze
#pragma cake diagnostic check "-Wmissing-owner-qualifier"

