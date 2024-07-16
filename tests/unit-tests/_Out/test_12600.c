#pragma safety enable



void* /*_Owner*/ /*_Opt*/  malloc(unsigned long size);
void free(void* /*_Owner*/ /*_Opt*/  ptr);

void f1()
{
    void * /*_Owner*/ /*_Opt*/ p = malloc(1);
    if (p) {
      
    }

    
    free(p);
}

void f2(int condition)
{
    void * /*_Owner*/ /*_Opt*/  p = malloc(1);
    if (condition) {
      
    }

    
    
}

void f3(int condition)
{
    void * /*_Owner*/ /*_Opt*/  p = malloc(1);

    if (condition) {
       free(p);
    }
    else {
       free(p);
    }

    
}

void f3(int condition)
{
    void * /*_Owner*/ /*_Opt*/  p = malloc(1);

    if (condition) {

    }
    else {
       free(p);
    }

    
    
}


void f4(int condition)
{
    void * /*_Owner*/ /*_Opt*/  p = malloc(1);

    if (condition) {
       free(p);
    }
    else {

    }

    
    
}

void f5(int condition)
{
    void * /*_Owner*/ /*_Opt*/  p = malloc(1);

    if (p) {
       free(p);
       return;
    }

    
}
