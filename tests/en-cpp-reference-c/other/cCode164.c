//en.cppreference.com/w/c/memory/free_sized.html
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
 
typedef struct
{
    size_t size;     // current number of elements
    size_t capacity; // reserved number of elements
    void** data;
} PtrVector;
 
PtrVector vector_create(size_t initial_capacity)
{
    PtrVector ret =
    {
        .capacity = initial_capacity,
        .data = (void**) malloc(initial_capacity * sizeof(void*))
    };
    return ret;
}
 
void vector_delete(PtrVector* self)
{
    free_sized(self->data, self->capacity * sizeof(void*));
}
 
void vector_push_back(PtrVector* self, void* value)
{
    if (self->size == self->capacity)
    {
        self->capacity *= 2;
        self->data = (void**) realloc(self->data, self->capacity * sizeof(void*));
    }
    self->data[self->size++] = value;
}
 
int main()
{
    int data = 42;
    float pi = 3.141592f;
    PtrVector v = vector_create(8);
    vector_push_back(&v, &data);
    vector_push_back(&v, &pi);
    printf("data[0] = %i\n", *(int*)v.data[0]);
    printf("data[1] = %f\n", *(float*)v.data[1]);
    vector_delete(&v);
}