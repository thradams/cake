#pragma safety enable

void free(void* /*_Owner*/ /*_Opt*/ p);
char* /*_Owner*/ /*_Opt*/ strdup(const char* s);

struct X {
    char* /*_Owner*/ /*_Opt*/ text;
};

struct X* /*_Owner*/ make();

void f(int condition)
{
    struct X* /*_Owner*/ /*_Opt*/ p1 = make();
    
    
    {
        struct X* /*_Owner*/ /*_Opt*/ p2 = make();

        struct X* /*_Owner*/ /*_Opt*/ p = ((void*)0);
        if (condition)
        {
            p = p1;
        }
        else
        {
            p = p2;
        }

        free(p->text);
        p->text = strdup("c");

        free(p->text);
        free(p);
    }
#pragma cake diagnostic check "-Wmissing-destructor"

}
#pragma cake diagnostic check "-Wmissing-destructor"
