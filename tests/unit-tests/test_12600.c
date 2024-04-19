

void* _Owner malloc(unsigned long size);
void free(void* _Owner ptr);

void f1()
{
    void * _Owner p = malloc(1);
    if (p) {
      static_state(p, "not-null ");
    }

    static_state(p, "maybe-null ");
    free(p);
}

void f2(int condition)
{
    void * _Owner p = malloc(1);
    if (condition) {
      static_state(p, "maybe-null ");
    }

    static_state(p, "maybe-null ");
    static_set(p, "null");
}

void f3(int condition)
{
    void * _Owner p = malloc(1);

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
    void * _Owner p = malloc(1);

    if (condition) {

    }
    else {
       free(p);
    }

    static_state(p, "uninitialized  maybe-null ");
    static_set(p, "null");
}


void f4(int condition)
{
    void * _Owner p = malloc(1);

    if (condition) {
       free(p);
    }
    else {

    }

    static_state(p, "uninitialized maybe-null ");
    static_set(p, "null");
}

void f5(int condition)
{
    void * _Owner p = malloc(1);

    if (p) {
       free(p);
       return;
    }

    static_state(p, "null ");
}
