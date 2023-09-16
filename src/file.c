

void* owner malloc(unsigned long size);
void free(void* owner ptr);

void f1()
{
    void * owner p = malloc(1);
    if (p) {
      static_state(p, "not-null");
    }

    static_state(p, "maybe-null");
    free(p);
}

void f2(int condition)
{
    void * owner p = malloc(1);
    if (condition) {
      static_state(p, "maybe-null");
    }

    static_state(p, "maybe-null");
    static_set(p, "null");
}

void f3(int condition)
{
    void * owner p = malloc(1);
    
    if (condition) {
       free(p);
    }
    else {
       free(p);
    }

    static_state(p, "uninitialized");    
}

void f3(int condition)
{
    void * owner p = malloc(1);
    
    if (condition) {
       
    }
    else {
       free(p);
    }

    static_state(p, "uninitialized or maybe_null");    
    static_set(p, "null");
}


void f4(int condition)
{
    void * owner p = malloc(1);
    
    if (condition) {
       free(p);
    }
    else {
       
    }

    static_state(p, "uninitialized or maybe_null");   
    static_set(p, "null");
}

void f5(int condition)
{
    void * owner p = malloc(1);
    
    if (p) {
       free(p);
       return;
    }
    
    static_state(p, "null");    
}

