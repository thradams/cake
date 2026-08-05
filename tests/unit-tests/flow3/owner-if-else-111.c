#pragma safety enable



void* _Owner _Opt _Uninitialized malloc(unsigned long size);
void free(void* _Owner _Opt  ptr);

void f1()
{
    void * _Owner _Opt p = malloc(1);
    if (p) {
      compile_assert(p != 0);
    }

    //assert_state(p, "null | not-null ");
    free(p);
}

void f2(int condition)
{
    void * _Owner _Opt  p = malloc(1); // owner object (p) not moved (call-opt-nonnull see line 21)
    if (condition) {
      //assert_state(p, "null | not-null ");
    }

    //assert_state(p, "null | not-null ");
    override_state(p, "null");
} //lint 29 owner object (p) not moved (call-opt-nonnull see line 21)

void f3(int condition)
{
    void * _Owner _Opt  p = malloc(1);

    if (condition) {
       free(p);
    }
    else {
       free(p);
    }

    //assert_state(p, "uninitialized");
}

void f4(int condition)
{
    void * _Owner _Opt  p = malloc(1); // owner object (p) not moved (var-true see line 46)

    if (condition) {

    }
    else {
       free(p);
    }

    //assert_state(p, "uninitialized  null | not-null ");
    override_state(p, "null");
} //lint 29 owner object (p) not moved (var-true see line 46)


void f5(int condition)
{
    void * _Owner _Opt  p = malloc(1); // owner object (p) not moved (var-false see line 62)

    if (condition) {
       free(p);
    }
    else {

    }

    //assert_state(p, "uninitialized null | not-null ");
    override_state(p, "null");
} //lint 29 owner object (p) not moved (var-false see line 62)

void f6(int condition)
{
    void * _Owner _Opt  p = malloc(1);

    if (p) {
       free(p);
       return;
    }

    compile_assert(p == 0);
}
