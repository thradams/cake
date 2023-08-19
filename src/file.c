void * owner malloc(int sz);
void free(explicit void * owner opt p);


void f(int i) {   
    void * owner p = 0;
    if (i)
    {
        if (i)
        {
            p = move malloc(1);
        }
        else
        {
            p = move malloc(1);
        }     
    }
    
    free(p);
}

