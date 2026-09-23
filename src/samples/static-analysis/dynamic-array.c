#pragma safety enable

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

struct int_array {
    int* _Owner _Opt data;
    int size;
    int capacity;
};

int int_array_reserve(struct int_array* p, int n)
{
    if (n > p->capacity) {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0])))) {
            return EOVERFLOW;
        }

        void* _Owner _Opt pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;
        
        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int int_array_push_back(struct int_array* p, int value)
{
    if (p->size == INT_MAX) {
        return EOVERFLOW;
    }

    if (p->size + 1 > p->capacity) {
        int new_capacity = 0;
        if (p->capacity > (INT_MAX - p->capacity / 2))
        {
            /*overflow*/
            new_capacity = INT_MAX;
        }
        else {
            new_capacity = p->capacity + p->capacity / 2;
            if (new_capacity < p->size + 1) {
                new_capacity = p->size + 1;
            }
        }

        int error = int_array_reserve(p, new_capacity);
        if (error != 0) {
            return error;
        }
    }

    p->data[p->size] = value;
    p->size++;

    return 0;
}

void int_array_destroy(_Dtor struct int_array* p)
{
    free(p->data);
}

int main()
{
    struct int_array a = { 0 };
    int_array_push_back(&a, 1);
    int_array_push_back(&a, 2);
    int_array_destroy(&a);
}
