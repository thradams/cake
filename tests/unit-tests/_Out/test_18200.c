#pragma safety enable

void* /*_Owner*/ /*_Opt*/ malloc(unsigned long size);
void free(void* /*_Owner*/ /*_Opt*/ ptr);

int main()
{
    void* /*_Opt*/ p = malloc(1);

//left object must be an owner reference.
#pragma cake diagnostic check "-Wtemp-owner"

    free(p);
#pragma cake diagnostic check "-Wnon-owner-to-owner-move"    
}



