#pragma safety enable

/* the member and result defaults hold when the base type is a typedef; warning 122 is reported once per declaration */

typedef unsigned short uint16_t;
typedef unsigned long size_t;

typedef struct
{
    uint16_t* data; /* nullable by default */
} key1;

typedef struct
{
    uint16_t* _Nullable data; //lint 122
} key2;

typedef struct
{
    uint16_t* _Nonnull data;
} key3;

void* _Nullable my_realloc(void* _Nullable ptr, size_t new_size); //lint 122

/* a pointer typedef brings its own annotation: not reported here */
typedef int* _Nullable nullable_int_ptr;
struct holder { nullable_int_ptr p; };

void f(key1* a, key2* b, key3* c)
{
    *a->data = 1; //lint 33
    *b->data = 1; //lint 33
    *b->data = 2; //lint 33
    *c->data = 1;
}
