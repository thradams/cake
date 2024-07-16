#pragma safety enable


void* /*_Owner*/ malloc(unsigned size);
void free(void* /*_Owner*/ ptr);

struct X {
    char* /*_Owner*/ name;
};

struct X* /*_Owner*/ /*_Opt*/ F(int i)
{

    struct X* /*_Owner*/ /*_Opt*/ p1 = 0;
    if (1) /*try*/
    {
        if (i == 1)
        {
            p1 = malloc(sizeof * p1);            
            while (0){}            
        }
        else if (i == 3)
        {
            p1 = malloc(sizeof * p1);
        }
    }
    else _catch_label_1:
    {
    }

    return p1;  //p1.name not initialized
}

#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized]"

 