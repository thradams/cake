#pragma safety enable



void* _Owner _Opt  malloc(unsigned long size);
void free(void* _Owner _Opt  ptr);

void f1()
{
    void * _Owner _Opt p = malloc(1);
    if (p) {
      static_state(p, "not-null ");
    }

    static_state(p, "null | not-null ");
    free(p);
}

void f2(int condition)
{
    void * _Owner _Opt  p = malloc(1);
    if (condition) {
      static_state(p, "null | not-null ");
    }

    static_state(p, "null | not-null ");
    static_set(p, "null");
}

void f3(int condition)
{
    void * _Owner _Opt  p = malloc(1);

    if (condition) {
       free(p);
    }
    else {
       free(p);
    }

    static_state(p, "uninitialized");
}

void f4(int condition)
{
    void * _Owner _Opt  p = malloc(1);

    if (condition) {

    }
    else {
       free(p);
    }

    static_state(p, "uninitialized  null | not-null ");
    static_set(p, "null");
}


void f5(int condition)
{
    void * _Owner _Opt  p = malloc(1);

    if (condition) {
       free(p);
    }
    else {

    }

    static_state(p, "uninitialized null | not-null ");
    static_set(p, "null");
}

void f6(int condition)
{
    void * _Owner _Opt  p = malloc(1);

    if (p) {
       free(p);
       return;
    }

    static_state(p, "null ");
}
