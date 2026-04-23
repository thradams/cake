

#include <stdio.h>
#include <stdlib.h>

struct X {
    int count;
    double values[]; // flexible array
};

/*
    The size of a structure with a flexible array member is
    determined as if the flexible array member were omitted,
    EXCEPT that it may have more trailing padding than the
    omission would imply
*/

int main()
{

    int n = 3;

    printf("sizeof(struct X) = %d\n", (int)sizeof(struct X));
    printf("allocated = %d\n", (int)sizeof(struct X) + n * sizeof(double));

    struct X* p = malloc(sizeof(struct X) + n * sizeof(double));
    if (p == NULL) return 0;

    p->count = n;
    p->values[0] = 10.0;
    p->values[1] = 20.0;
    p->values[2] = 30.0;

    for (int i = 0; i < p->count; ++i)
        printf("%f\n", p->values[i]);

    free(p);

    return 0;
}
