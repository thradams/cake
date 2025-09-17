
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

struct int_array {
    int* data;
    int size;
    int capacity;
};

#def reserve(P, N)
(int(typeof(P) p, int n))
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(int))))
        {
            return EOVERFLOW;
        }

        void* pnew = realloc(p->data, n * sizeof(int));
        if (pnew == NULL) return ENOMEM;

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}(P, N)
#enddef

#def push(P, N)
(int(typeof(P) p, typeof((P)->data[0])  value))
{
    if (p->size == INT_MAX)
    {
        return EOVERFLOW;
    }

    if (p->size + 1 > p->capacity)
    {
        int new_capacity = 0;
        if (p->capacity > (INT_MAX - p->capacity / 2))
        {
            /*overflow*/
            new_capacity = INT_MAX;
        }
        else
        {
            new_capacity = p->capacity + p->capacity / 2;
            if (new_capacity < p->size + 1)
            {
                new_capacity = p->size + 1;
            }
        }

        int error = reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }

    p->data[p->size] = value;


    p->size++;

    return 0;
}(P, N)
#enddef

void int_array_destroy(struct int_array* p)
{
    free(p->data);
}

int main()
{
    struct int_array a = { 0 };


    reserve(&a, 1);
    push(&a, 1);
    push(&a, 2);
    //int_array_destroy(&a);
}
