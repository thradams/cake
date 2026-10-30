#pragma safety enable

void free(void* _Owner _Opt p);
char* _Owner strdup(const char* s);

struct X {
    char* _Owner text;
};

struct X* _Nonnull _Owner make();

void f(int condition)
{
    struct X* _Owner p1 = make(); // owner object (p1) not moved (var-false see line 14)

    {
        struct X* _Owner p2 = make(); // owner object (p2) not moved (var-true see line 17)

        struct X* _Owner p = nullptr;
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
    } //lint 29 owner object (p2) not moved (var-true see line 17)
} //lint 29 owner object (p1) not moved (var-false see line 14)
