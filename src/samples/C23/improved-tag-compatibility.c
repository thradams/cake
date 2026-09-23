//Improved tag compatibility
//https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3037.pdf

#include <stdio.h>

/*
  A struct/union/enum with the same tag and the same content is the same
  type everywhere in the translation unit, so a macro can define the type
  at each place it is used instead of requiring one typedef per instance.
*/
#define VEC(T) struct vec_##T { T* data; int size; int capacity; }

/* struct vec_int is defined here, inside the parameter list (prototype scope) */
void push_int(VEC(int)* v, int x)
{
    v->data[v->size++] = x;
}

int main()
{
    int storage[4];

    /* C23: same tag + same content = same type as the parameter above.
       C17: a new, incompatible struct vec_int -> error on the call */
    VEC(int) v = { storage, 0, 4 };

    push_int(&v, 1);
    push_int(&v, 2);

    printf("%d %d\n", v.data[0], v.data[1]);
}
