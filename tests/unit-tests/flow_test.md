# Flow Tests

This document collects 151 unit tests from `tests/unit-tests` whose filenames start with `flow_`.

## `flow_address_of.c`

Covers flow analysis behavior for address of.

<!-- runnable -->

```c
#pragma safety enable


struct X {
    char* _Owner p;
};
void x_destroy(_Dtor struct X* p);
void f(struct X* x)
{

    x_destroy(x); //lint 19 source expression of _Dtor must be addressof
}
```

## `flow_always_null_check.c`

Covers flow analysis behavior for always null check.

<!-- runnable -->

```c
#pragma safety enable

void f(int* p)
{

    if (p != 0) //lint 28 pointer is always non-null   
    {
    } 

    //pointer is always non-null

    if (0 != p)  //lint 28 pointer is always non-null   
    {}

    //pointer is always non-null

    if (p == 0)  //lint 28 pointer is always non-null   
    {}


    if (0 == p)  //lint 28 pointer is always non-null   
    {}
}
```

## `flow_and.c`

Covers flow analysis behavior for and.

<!-- runnable -->

```c
#pragma safety enable


int* _Opt f();

int main()
{
    int* _Opt p1 = f();
    int* _Opt p2 = f();

    if (p1 && p2)
    {
        assert_state(p1, "not-null");
        assert_state(p2, "not-null");
    }
    else
    {
    }
}
```

## `flow_and_expr.c`

Covers flow analysis behavior for and expr.

<!-- runnable -->

```c
#pragma safety enable

char* _Opt f();
int strcmp(const char * s1, const char *s2);
void g()
{
    const char* _Opt s1 = f();    
    if (s1 && strcmp(s1, "a")==0)
    {
        assert_state(s1, "not-null");        
    }
    else
    {        
        assert_state(s1, "null not-null");
    }
}
```

## `flow_and_func.c`

Covers flow analysis behavior for and func.

<!-- runnable -->

```c
#pragma safety enable

int strcmp(const char* s1, const char* s2);
char* _Opt f();

int main()
{
    const char* _Opt s = f();
    if (s && strcmp(s, "a") == 0)
    {
    }
    else if (s)
    {
    }
}
```

## `flow_and_or_expr.c`

Covers flow analysis behavior for and or expr.

<!-- runnable -->

```c
#pragma safety enable


struct X {
    int i;
};

void f(struct X* _Opt p)
{
    if (p && p->i || p->i > 2) //lint 33 pointer may be null
    {
    }
    else
    {
    }
}
```

## `flow_and_ptr.c`

Covers flow analysis behavior for and ptr.

<!-- runnable -->

```c
#pragma safety enable


struct X {
    int i;
};

void f(struct X * _Opt p)
{
      if (p && p->i)
      {          
      }
      else
      {          
      }
}
```

## `flow_assert_func2.c`

Covers flow analysis behavior for assert func2.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);
char* _Owner _Opt strdup(const char* s);


struct X {

    char* _Owner _Opt text;
};
#define NULL ((void*)0)

struct X makex();
void clear(struct X* p);
void f(struct X* _Opt pX)
{
    struct X x = makex();
    clear(&x);
    assert(x.text == 0);
}
```

## `flow_assert_function.c`

Covers flow analysis behavior for assert function.

<!-- runnable -->

```c
struct X { const char* _Owner text; };
void destroyX(struct X x) 
{
	assert(x.text == 0);
}

int main() 
{
}
```

## `flow_assigment_inside_expr.c`

Covers flow analysis behavior for assignment inside expr.

<!-- runnable -->

```c
#pragma safety enable

/*
  returns a non-null pointer
*/
int* f();

int main()
{
    int* p;

    if ((p = f()) == 0) //lint 28 pointer is always non-null
    {
        //if will not change the state
        assert_state(p, "not-null");
    }
    else
    {
        assert_state(p, "not-null");
    }
}
```

## `flow_assignment_compound.c`

Covers flow analysis behavior for assignment compound.

<!-- runnable -->

```c
struct X { int i; void* p; };
int main() {
	struct X x;
	x = (struct X){ 0 };
	assert_state(x.i, "zero");
	assert_state(x.p, "null");
}
```

## `flow_calloc.c`

Covers flow analysis behavior for calloc.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner calloc(unsigned long n , unsigned long size);
void free(void* _Owner _Opt ptr);

struct Y {
  char * _Owner _Opt p0;
  int * _Owner _Opt p2;
  double i2;
};

struct X {
  char * _Owner _Opt text;
  int * _Owner _Opt p1;
  int i;
  struct Y  * _Opt pY;
};

int main() {   
   struct X * _Owner x = calloc(1,sizeof * x);
   assert_state(x, "null | not-null ");

   assert_state(x->p1, "null ");
   assert_state(x->i, "zero");
   assert_state(x->pY, "null");
   assert_state(x->pY->p0, "");
   assert_state(x->pY->p2, "");
   assert_state(x->pY->i2, "");   
   free(x);
}
```

## `flow_calloc2.c`

Covers flow analysis behavior for calloc2.

<!-- runnable -->

```c
#pragma safety enable



struct X { int i; void* _Opt p; };
void* _Owner _Opt calloc(int i, int sz);
void free(void* _Owner _Opt p);

int main()
{
    struct X* _Owner _Opt p = calloc(1, 1);
    assert_state(p, "null | not-null ");
    if (p)
    {
    assert_state(p->i, "zero");
    assert_state(p->p, "null");
    }
    free(p);
}
```

## `flow_circular_refs.c`

Covers flow analysis behavior for circular refs.

<!-- runnable -->

```c
#pragma safety enable


struct item
{
    struct item* _Opt other;
};

struct items
{
    struct item * _Opt one;
    struct item * _Opt two;
};

void f(struct items i){}

int main()
{
    struct item one = {0};
    struct item two = {0};
    struct items items = {0};
    items.one = &two;
    items.two = &one;
    one.other = &two;
    two.other = &one;
    f(items);
}
```

## `flow_const_arg.c`

Covers flow analysis behavior for const arg.

<!-- runnable -->

```c
#pragma safety enable



struct X {
  void * _Owner _Opt text;
};

void f(const struct X* list);

int main()
{
  struct X x = {};
  f(&x);
}
```

## `flow_const_arg2.c`

Covers flow analysis behavior for const arg2.

<!-- runnable -->

```c
#pragma safety enable



void free(void* _Owner p);
struct X
{
    int i;
    void* _Owner _Opt p;
};
void f(const struct X* _Opt p);
int main()
{
    struct X x = { 0 };
    f(&x);
    assert_state(x.p, "null");
}
```

## `flow_default_init.c`

Covers flow analysis behavior for default init.

<!-- runnable -->

```c
#pragma safety enable


struct Y {
  char * _Owner p0;
  int * _Owner p2;
  double i2;
};

struct X {
  char * _Owner text;
  int * _Owner p1;
  int i;
  struct Y  *pY;
};

int main() {   
   _Opt struct X x = {0};   
   assert_state(x.text, "null");
   assert_state(x.p1, "null");
   assert_state(x.i, "zero");
   assert_state(x.pY, "null");
}
```

## `flow_default_init_notnull.c`

Covers flow analysis behavior for default init notnull.

<!-- runnable -->

```c
#pragma safety enable;

struct X
 {
   char * _Owner _Opt p;
   char * p2;
 };

 char* _Owner _Opt strdup(const char *s);
 void free(void* _Owner _Opt p);

 int main() {
     const char* _Owner _Opt p0 = strdup("a");
     struct X x = {
         .p = p0
     };         
     free(x.p);
 }
```

## `flow_delete_destroy.c`

Covers flow analysis behavior for delete destroy.

<!-- runnable -->

```c

#pragma safety enable



void free( void* _Owner ptr);
void* _Owner malloc(int size);
struct X { char * _Owner text; };

void x_destroy(_Dtor struct X*  p)
{
    free(p->text);
}

void x_delete(struct X* _Owner _Opt p)
{
    if (p)
    {
        x_destroy(p);
        free(p);
    }
}
```

## `flow_destroy_opt.c`

Covers flow analysis behavior for destroy opt.

<!-- runnable -->

```c
#pragma safety enable


void free(void * _Owner _Opt p);
struct X {
    char * _Owner text;
};

void x_destroy(_Opt _Dtor struct X *  x) {
    free(x->text);
}

int main() {
    _Opt struct X x = {};
    x_destroy(&x);
}
```

## `flow_destructor.c`

Covers flow analysis behavior for destructor.

<!-- runnable -->

```c
#pragma safety enable


void free( void * _Owner p);

struct X {
  char * _Owner name;
};

void x_destroy( _Dtor struct X *  p);

struct Y {
  struct X x;
};

void y_destroy(_Dtor struct Y *  p) {
   x_destroy(&p->x);
}
```

## `flow_div_by_zero.c`

Covers flow analysis behavior for div by zero.

<!-- runnable -->

```c

#pragma safety enable

void f(int i)
{
    if (i < 0)
    {
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i; //lint 36 possible division by zero
    }

    if (0 > i)
    {
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i; //lint 36 possible division by zero
    }

    if (0 < i)
    {
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i; //lint 36 possible division by zero
    }

    if (i > 0)
    {
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i; //lint 36 possible division by zero
    }
}


void f2(int i)
{
    if (i <= 0)
    {
        int k = 1 / i; //lint 36 possible division by zero
    }
    else
    {
        int j = 1 / i;

    }

    if (0 >= i)
    {
        int k = 1 / i; //lint 36 possible division by zero
    }
    else
    {
        int j = 1 / i;
    }

    if (0 <= i)
    {
        int k = 1 / i; //lint 36 possible division by zero
    }
    else
    {
        int j = 1 / i;

    }


    if (i >= 0)
    {
        int k = 1 / i; //lint 36 possible division by zero
    }
    else
    {
        int j = 1 / i;

    }
}
```

## `flow_div_by_zero2.c`

Covers flow analysis behavior for div by zero2.

<!-- runnable -->

```c

#pragma safety enable

void  f(int i)
{
    int k = 1;
    int* p = &k;

    if (i)
    {
        *p = 0;
    }

    int h = 212 / k; //lint 36 possible division by zero
}

int main() {}
```

## `flow_do_while.c`

Covers flow analysis behavior for do while.

<!-- runnable -->

```c
#pragma safety enable



void* _Owner _Opt malloc(unsigned size);
void free(void* _Owner _Opt  ptr);

int main() {
   void * _Owner _Opt p = malloc(1);
   do{
      free(p);
   }
   while(0);   
}
```

## `flow_else.c`

Covers flow analysis behavior for else.

<!-- runnable -->

```c
#pragma safety enable

void* _Owner _Opt malloc(int sz);

void f(int i)
{
    if (i)
    {
    }
    else
    {
        int* _Owner _Opt p3 = malloc(1);
    } //lint 29 memory leak
}
```

## `flow_empty_init.c`

Covers flow analysis behavior for empty init.

<!-- runnable -->

```c
#pragma safety enable


struct Y {
  char * _Owner p0;
  int * _Owner p2;
  double i2;
};

struct X {
  char * _Owner text;
  int * _Owner p1;
  int i;
  struct Y  *pY;
};

int main() {   
   _Opt struct X x = {};   
   assert_state(x.text, "null");
   assert_state(x.p1, "null");
   assert_state(x.i, "zero");
   assert_state(x.pY, "null");
}
```

## `flow_extern_var.c`

Covers flow analysis behavior for extern var.

<!-- runnable -->

```c
#pragma safety enable

int g = 1;

void f1()
{
     //externa non constant variable are any state
     assert_state(g, "any");
     g  = 1;
     assert_state(g, "not-zero");
     g = 0;
     assert_state(g, "zero");     
}

void f2()
{
     assert_state(g, "any");
     g = 2;
     g = 3;
}
```

## `flow_for_loop.c`

Covers flow analysis behavior for for loop.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

int main()
{
    void* _Owner _Opt p = 0;
    for (int i = 0; i < 2; i++)
    {

        //object 'p' may not be empty
        
        p = malloc(1); //lint 29

    }
    free(p);
}
```

## `flow_for_loop2.c`

Covers flow analysis behavior for for loop2.

<!-- runnable -->

```c
#pragma safety enable

struct X {
    struct X* _Opt next;
};


void f(struct X* p);
struct X* _Opt get();

int c;

int main()
{
    struct X* _Opt p = get();
    assert_state(p, "null not-null");
    for (; p; p = p->next)
    {
        assert_state(p, "not-null");
    }
    assert_state(p, "null");

    for (p = get(); p; p = p->next)
    {
        assert_state(p, "not-null");
    }
    assert_state(p, "null");

    for (p = get(); p; )
    {
        assert_state(p, "not-null");
        p = p->next;
    }
    assert_state(p, "null");

    for (p = get(); p; )
    {
        if (c)
        {
            assert_state(p, "not-null");
            break;
        }
        p = p->next;
    }
    assert_state(p, "null not-null");

    for (struct X* _Opt p2 = get(); p2; )
    {
        assert_state(p2, "not-null");
        f(p2);
    }

    
    struct X* _Opt p3 = get();
    for (;;)
    {
        break;
    }
    assert_state(p3, "null not-null");

}
```

## `flow_for_loop_j.c`

Covers flow analysis behavior for for loop j.

<!-- runnable -->

```c
int main()
{
  int j;
  for (j = 0; j <10; j++) {}
  return j;
}
```

## `flow_for_return.c`

Covers flow analysis behavior for for return.

<!-- runnable -->

```c
#pragma safety enable

struct X {
    int i;
};

void fill_options(struct X* options)
{
    for (int i = 1; i < 2; i++)
    {
        options->i = 1; //cannot say options end lifetime (caused by return)
        return;
    }    
}
```

## `flow_free_moved.c`

Covers flow analysis behavior for free moved.

<!-- runnable -->

```c
#pragma safety enable

void* _Owner _Opt calloc(unsigned long n, unsigned long size);
void free(void* _Owner _Opt ptr);


int main()
{
    int* _Owner _Opt p = calloc(1, sizeof(int));
    int* _Owner _Opt p2 = p;    //MOVED
    free(p2);

    //object may be already moved
    free(p); //lint 32

}
```

## `flow_function_call_order.c`

Covers flow analysis behavior for function call order.

<!-- runnable -->

```c
#pragma safety enable

struct X {
  int * _Opt p;
};

struct X * make();

void f(struct X *p,  int * p);

int main()
{
    struct X *  pX = make();
    if (pX->p)
    {
       //cake is making pX->p  unkown before function call..it must be after
       f(pX, pX->p);
    }
}
```

## `flow_if_else.c`

Covers flow analysis behavior for if else.

<!-- runnable -->

```c
#pragma safety enable



void * _Owner _Opt  malloc(int sz);
void free( void * _Owner _Opt p);


void f(int i) {   
    void * _Owner _Opt p = 0;
    if (i)
    {
        if (i)
        {
            p =  malloc(1);
        }
        else
        {
            p = malloc(1);
        }     
    }
    
    free(p);
}
```

## `flow_if_else2.c`

Covers flow analysis behavior for if else2.

<!-- runnable -->

```c
#pragma safety enable


int rand();
void free(void* _Owner ptr);

struct X { char* _Owner _Opt name; };

void x_destroy(_Dtor struct X*  p);
struct X f();

void g()
{
    {
        struct X x = {0};   
        
        if (rand())
        {            
            x = f();
        }
        else
        {         
            x = f();
        }
        x_destroy(&x);
    }
}
```

## `flow_if_else3.c`

Covers flow analysis behavior for if else3.

<!-- runnable -->

```c
#pragma safety enable

void free(void *_Owner _Opt p);
char * _Owner _Opt strdup(const char* s);

struct X {
  char *_Owner _Opt text;
};

struct X * _Owner make();

void f(int condition)
{
    struct X * _Owner _Opt p = nullptr;
    if (condition)
    {
        p = make();        
    }
    else
    {
        p = make();        
    }
    
    free(p->text);
    p->text = strdup("c");
    
    free(p->text);
    free(p);
}
```

## `flow_if_ptr_if_ptr.c`

Covers flow analysis behavior for if ptr if ptr.

<!-- runnable -->

```c
#pragma safety enable

struct Z {
    int i;
};

struct Y {
    struct Z* _Opt pZ;
};

struct X {
    struct Y* _Opt pY;
};

void f(struct X* _Opt p)
{
    if (p && p->pY && p->pY->pZ)
    {
        p->pY->pZ->i = 1;
    }
}
```

## `flow_if_return_else.c`

Covers flow analysis behavior for if return else.

<!-- runnable -->

```c
#pragma safety enable

    
void * _Owner _Opt calloc(int i, int n);
void free(void * _Owner _Opt p);

int main() {
    int * _Owner _Opt p1 = 0;
    int * _Owner _Opt p2 = calloc(1, sizeof(int));

    if (p2 == 0) {
        return 1;
    }
    else
    {
      p1 = p2;
    }
    assert_state(p2, "moved");
    free(p1);
    return 0;
}
```

## `flow_if_return_else_return.c`

Covers flow analysis behavior for if return else return.

<!-- runnable -->

```c
#pragma safety enable

void f(int i, int c)
{
    if (c < 4)
    {
        if (c < 1)
        {
           return;
        }
        else
        {
            return;
        }
    }

    i = 0; //saying i end of lifetime
}
```

## `flow_if_throw.c`

Covers flow analysis behavior for if throw.

<!-- runnable -->

```c
#pragma safety enable



int f();
int main()
{
    int i;
    try
    {
        if (f()){
            i = 1;
            throw;
        }
        i = 0;
    }
    catch
    {
        assert_state(i, "not-zero");
    }
    assert_state(i, "zero | not-zero");
}
```

## `flow_initialization_leak.c`

Covers flow analysis behavior for initialization leak.

<!-- runnable -->

```c
#pragma safety enable;

struct X
{
    char* _Owner _Opt p;
};

char* _Owner _Opt strdup(const char* s);

int main()
{
    struct X x  = {
        .p = strdup("a")
    };
} //lint 29 leak x.p
```

## `flow_initialization_move.c`

Covers flow analysis behavior for initialization move.

<!-- runnable -->

```c
﻿#pragma safety enable;

struct X
 {
   char * _Owner _Opt p;
 };

 char* _Owner _Opt strdup(const char *s);
 void free(void* _Owner _Opt p);

 int main() {
     const char* _Owner _Opt p0 = strdup("a");
     struct X x = {
         .p = p0
     };         
     free(x.p);
 }
```

## `flow_inside_if.c`

Covers flow analysis behavior for inside if.

<!-- runnable -->

```c
#pragma safety enable


void * _Owner malloc(int i);
void free( void * _Owner _Opt p);

struct X {
  char * _Owner name;    
};

int main() {
   struct X * _Owner p = malloc(sizeof * p);
   if (p) {
     p->name = malloc(10);
     free(p->name);
   }
   free(p);   
}
```

## `flow_int_owner.c`

Covers flow analysis behavior for int owner.

<!-- runnable -->

```c
#pragma safety enable


void F(int i);
_Owner int make();
int main()
{
    F(make()); //lint 23 cannot assign a temporary owner to non-owner object
}
```

## `flow_leak_assigment.c`

Covers flow analysis behavior for leak assignment.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner _Opt malloc(int i);
void free(void* _Owner p);

struct X {
    char* _Owner name;
};

int main()
{
    struct X* _Owner _Opt p = malloc(sizeof * p);

    //object 'p' may not be empty

    p = 0; //lint 29
}
```

## `flow_leak_else.c`

Covers flow analysis behavior for leak else.

<!-- runnable -->

```c
#pragma safety enable



void free(void* _Owner _Opt ptr);
void* _Owner _Opt malloc(int size);

void f(int c)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    if (c)
    {
        free(p);
    }
} //lint 29 leak
```

## `flow_leak_goto.c`

Covers flow analysis behavior for leak goto.

<!-- runnable -->

```c
#pragma safety enable


void free(void* _Owner _Opt ptr);
void* _Owner _Opt malloc(int size);

void f(int condition)
{
    int* _Owner _Opt p = malloc(sizeof(int));

    if (condition)
    {
        goto end;
    }

    free(p);
end:
} //lint 29 leak p
```

## `flow_leak_goto2.c`

Covers flow analysis behavior for leak goto2.

<!-- runnable -->

```c
#pragma safety enable


void free(void* _Owner _Opt ptr);
void* _Owner _Opt malloc(int size);

void f(int condition)
{
    int* _Owner _Opt p = malloc(sizeof(int));

    if (condition)
        goto end; /*memory leak o p*/

    free(p);
end:

} //lint 29 leak
```

## `flow_leak_if.c`

Covers flow analysis behavior for leak if.

<!-- runnable -->

```c
#pragma safety enable

void* _Owner _Opt f();
void free(void* _Owner p);
int main()
{
    void* _Owner _Opt p  = f();
    if (p)
    {
        free(p);
        p = f();
    }
} //lint 29 memory leak
```

## `flow_leak_member.c`

Covers flow analysis behavior for leak member.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner _Opt calloc(int n, int size);

struct X {
    char* _Owner _Opt name;
};

void* _Owner _Opt f1()
{
    struct X* _Owner _Opt p = calloc(1, sizeof(struct X));
    if (p)
    {
        p->name = calloc(1, 2);
    }

    //
    return p; //lint 29  object 'name' may not be empty
}
```

## `flow_leak_non_pointer.c`

Covers flow analysis behavior for leak non pointer.

<!-- runnable -->

```c
#pragma safety enable

struct X {
    int _Owner i;
};

void f()
{
    const struct X x  = {0};
} //lint 29 x.i
```

## `flow_leak_struct.c`

Covers flow analysis behavior for leak struct.

<!-- runnable -->

```c
#pragma safety enable



char* _Owner strdup(const char* s);
void free(void* _Owner p);

struct X
{
    char* _Owner _Opt text;
};

void f(int a)
{
    struct X x  = {0};
    x.text = strdup("a");
} //lint 29 leak
```

## `flow_lifetime_ended.c`

Covers flow analysis behavior for lifetime ended.

<!-- runnable -->

```c
#pragma safety enable

struct X {
    int i;
};

void f2()
{
    struct X* _Opt p = 0;
    {
        struct X x = {0};
        p = &x;
    }

    if (p->i)  //lint 31  object lifetime ended 
    {
    }
}
```

## `flow_linked_list_destroy.c`

Covers flow analysis behavior for linked list destroy.

<!-- runnable -->

```c
#pragma safety enable


struct item {
    struct item* _Owner _Opt next;
};
void item_delete(struct item* _Owner p);

struct list {
    struct item* _Owner _Opt head;
    struct item* _Opt tail;
};

void list_destroy(_Dtor struct list*  list)
{
    struct item* _Owner _Opt p = list->head;
    while (p)
    {
        struct item* _Owner _Opt next = p->next;
        p->next = 0;
        item_delete(p);
        p = next;
    }
}

int main()
{
    struct list list = { 0 };
    list_destroy(&list);
}
```

## `flow_linked_list_push.c`

Covers flow analysis behavior for linked list push.

<!-- runnable -->

```c
#pragma safety enable



#define NULL ((void*)0)

struct item {
  int i;
  struct item * _Owner _Opt next;
  struct item * _Opt previous;
};

struct list
{
    struct item* _Owner _Opt head;
    struct item* _Opt tail;
};
void list_push(struct list* list, struct item* _Owner pnew)
{
    if (list->head == NULL)
    {
        list->head = pnew;
        list->tail = pnew;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        pnew->previous = list->tail;
        list->tail->next = pnew;
        list->tail = pnew;
    }
    
}
```

## `flow_list.c`

Covers flow analysis behavior for list.

<!-- runnable -->

```c
#pragma safety enable


void free(void* _Owner _Opt p);

struct token
{
    int type;
    struct token* _Opt next;
    struct token* _Opt prev;
};

void print_line(struct token* p)
{
    struct token* _Opt prev = p->prev;
    if (prev)
    {
        struct token* _Opt next = prev;
        while (next && next->type != 0)
        {
            next = next->next;
        }
    }
}
```

## `flow_lvalue_error3.c`

Covers flow analysis behavior for lvalue error3.

<!-- runnable -->

```c
int main()
{
    int* p = &1; //lint 1220 error: lvalue required as unary '&' operand 
}
```

## `flow_malloc.c`

Covers flow analysis behavior for malloc.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

struct Y {
  char * _Owner p0;
  int * _Owner p2;
  double i2;
};

struct X {
  char * _Owner text;
  int * _Owner p1;
  int i;
  struct Y  *pY;
};

int main() {   
   struct X * _Owner x = malloc(sizeof * x);
   assert_state(x, "null | not-null ");

   assert_state(x->p1, "uninitialized");
   assert_state(x->i, "uninitialized");
   assert_state(x->pY, "uninitialized");
   free(x);
}
```

## `flow_malloc_init.c`

Covers flow analysis behavior for malloc init.

<!-- runnable -->

```c
#pragma safety enable


struct X
{
    int i;
    void *p;
};
void *_Owner _Opt malloc(int i, int sz);
void free(void *_Owner _Opt p);

int main()
{
    struct X *_Owner _Opt p = malloc(1, 1);
    assert_state(p, "null | not-null ");
    if (p)
    {
        assert_state(p->i, "uninitialized");
        assert_state(p->p, "uninitialized");
    }
    free(p);
}
```

## `flow_maybe_moved.c`

Covers flow analysis behavior for maybe moved.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);
char* _Owner _Opt strdup(const char* s);

struct X {
    char* _Owner _Opt text;
};

struct X* _Owner make();

void f(int condition)
{
    struct X* _Owner _Opt p1 = make();

    {
        struct X* _Owner _Opt p2 = make();

        struct X* _Owner _Opt p = nullptr;
        if (condition)
        {
            p = p1;
        }
        else
        {
            p = p2;
        }

        free(p->text);
        p->text = strdup("c");

        free(p->text);
        free(p);
    } //lint 29 leak
} //lint 29 leak
```

## `flow_maybe_uninitialized.c`

Covers flow analysis behavior for maybe uninitialized.

<!-- runnable -->

```c
#pragma safety enable

int printf(const char* restrict format, ...);

struct X {
    int i;
};

//warning: uninitialized object 'x.i'

void f(_Ctor struct X* x, int i)
{
    if (i > 0)
    {
        x->i = 1;
    }
} //lint 30

//external variables, in this case object *x cannot leave scope
//with uninitialized values.

int main()
{
    struct X x;
    f(&x, 1);
    printf("%d", x.i);
}
```

## `flow_maybe_unitialized.c`

Covers flow analysis behavior for maybe uninitialized.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner _Opt malloc(unsigned long size);

struct X {
    char* _Owner text;
};

void x_delete(struct X* _Owner  _Opt p);

int main()
{
    struct X* _Owner _Opt p = malloc(sizeof(struct X));
    //warning: uninitialized object 'p.text'
    
    x_delete(p); //lint 30
}
```

## `flow_missing_cleanup_dtor.c`

Covers flow analysis behavior for missing cleanup dtor.

<!-- runnable -->

```c
#pragma safety enable



void free(void* _Owner _Opt p);

struct X {
    char* _Owner name;
};

void x_delete(struct X* _Owner _Opt p)
{
    if (p)
    {

        //object 'name' may not be empty

        free(p); //lint 29
    }
}
```

## `flow_move_inside_if.c`

Covers flow analysis behavior for move inside if.

<!-- runnable -->

```c
#pragma safety enable



void* _Owner calloc(unsigned n, unsigned size);
void free(void* _Owner ptr);

struct X {
    char* _Owner _Opt name;
};

struct X* _Owner _Opt F(int i)
{
    struct X* _Owner _Opt p1 = 0;
    struct X* _Owner _Opt p2 = calloc(1, sizeof * p2);
    if (p2)
    {
        p1 = p2;
    }
    return p1;
}
```

## `flow_move_return.c`

Covers flow analysis behavior for move return.

<!-- runnable -->

```c
#pragma safety enable


char * _Owner _Opt f() {
    char * _Owner _Opt p = 0;
    return p; /*implicit move*/
}
```

## `flow_move_struct_arg.c`

Covers flow analysis behavior for move struct arg.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner malloc(unsigned size);
void free(void* _Owner ptr);


struct X {  char *_Owner name; };
struct Y { struct X x; };

void f(struct Y * y, _Dtor struct X *  p) 
{
    free(y->x.name);
    y->x = *p;
}
```

## `flow_moved.c`

Covers flow analysis behavior for moved.

<!-- runnable -->

```c
#pragma safety enable

void free(void *_Owner _Opt p);

void f(int* _Opt _Owner p)
{
   int * _Opt _Owner p2 = p;   
   assert_state(p, "null, moved");
   if (p)
   {
     assert_state(p, "moved");
   }
   free(p2);
}
```

## `flow_no_leak.c`

Covers flow analysis behavior for no leak.

<!-- runnable -->

```c
#pragma safety enable


void free( void* _Owner ptr);
void* _Owner malloc(int size);

int main()
{
    int* _Owner p = malloc(sizeof(int));
    if (p)
    {
       free(p);     
    }
}
```

## `flow_no_leak2.c`

Covers flow analysis behavior for no leak2.

<!-- runnable -->

```c
#pragma safety enable


void free( void* _Owner ptr);
void* _Owner malloc(int size);

int main()
{
    int* _Owner p = malloc(sizeof(int));
    if (p != 0)
    {
       free(p);     
    }
}
```

## `flow_no_leak_delete.c`

Covers flow analysis behavior for no leak delete.

<!-- runnable -->

```c
#pragma safety enable


void free( void * _Owner p);
struct X {
  char * _Owner text;
};
void x_delete( struct X * _Owner _Opt p)
{
    if (p)
    {
      free(p->text);
      free(p);
    }
}
```

## `flow_no_leak_malloc.c`

Covers flow analysis behavior for no leak malloc.

<!-- runnable -->

```c
#pragma safety enable


void * _Owner _Opt calloc(int i, int sz);
void free( void * _Owner _Opt p);

struct X { int i; };
struct Y { struct X * _Owner _Opt p; };

int main() {
   struct Y y = {0};
   struct X * _Owner _Opt p = calloc(1, sizeof(struct X));
   if (p){
     y.p = p;
   }
  free(y.p);
}
```

## `flow_no_leak_struct.c`

Covers flow analysis behavior for no leak struct.

<!-- runnable -->

```c
#pragma safety enable


char * _Owner strdup(const char* s);
void free(void* _Owner p);

struct X {
  char * _Owner _Opt text;
};

void f(int a)
{
    struct X x = {0};
    x.text = strdup("a");
    free(x.text);
}
```

## `flow_no_return.c`

Covers flow analysis behavior for no return.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);
char* _Owner _Opt strdup(const char* s);
[[noreturn]] void exit( int exit_code );

void f()
{
    char * _Opt _Owner s = strdup("a");
    
    if (s == nullptr)
    {
        exit(1);
    }
    
    assert_state(s, "not-null");
    free(s);
}
```

## `flow_non_const_arg.c`

Covers flow analysis behavior for non const arg.

<!-- runnable -->

```c
#pragma safety enable


struct X {
    void* _Owner _Opt text;
};

void x_change(struct X* list);
void x_destroy(_Dtor struct X* p);

int main()
{
    //object pointed by 'x.text' was not released
    struct X x = {};
    x_change(&x);
} //lint 29
```

## `flow_non_const_arg2.c`

Covers flow analysis behavior for non const arg2.

<!-- runnable -->

```c
#pragma safety enable


void free(void* _Owner _Opt p);
struct X
{
    int i;
    void* _Owner _Opt p;
};
void f(struct X* _Opt p);
int main()
{
    struct X x = { 0 };
    assert_state(x.p, "null");
    f(&x);
    assert_state(x.p, "null | not-null");
    free(x.p);
}
```

## `flow_non_nullable_arg.c`

Covers flow analysis behavior for non nullable arg.

<!-- runnable -->

```c
#pragma safety enable

void f(int *p)
{
    assert_state(p, "not-null");
}
```

## `flow_non_nullable_arg2.c`

Covers flow analysis behavior for non nullable arg2.

<!-- runnable -->

```c
#pragma safety enable
void f(int  *p)
{
  assert_state(p, "not-null");
}
```

## `flow_non_nullable_return.c`

Covers flow analysis behavior for non nullable return.

<!-- runnable -->

```c
#pragma safety enable


int* f();
int main()
{
    int* _Opt p = f();


    if (p) //lint 28 pointer is always not-null
        return 0;

}
```

## `flow_non_owner_to_owner.c`

Covers flow analysis behavior for non owner to owner.

<!-- runnable -->

```c
#pragma safety enable



void* f();
int main()
{
    //cannot assign a non-owner to owner
    void* _Owner p  = f(); //lint 25
} //lint 29
```

## `flow_non_owner_to_owner2.c`

Covers flow analysis behavior for non owner to owner2.

<!-- runnable -->

```c
#pragma safety enable

void* f();

int main()
{
    //cannot assign a non-owner to owner

    void* _Owner p = f(); //lint 25
} //lint 29
```

## `flow_non_pointer_owner.c`

Covers flow analysis behavior for non pointer owner.

<!-- runnable -->

```c
#pragma safety enable


void destroy(_Owner int i);

int main()
{
    _Owner int i = 0;
    int v = i;

    //warning: cannot assign a non-owner to owner

    destroy(v); //lint 25

} //lint 29
```

## `flow_non_ptr_owner2.c`

Covers flow analysis behavior for non ptr owner2.

<!-- runnable -->

```c
#pragma safety enable


_Owner int socket();
void close(_Owner int fd);

int main()
{
  _Owner int fd;
  
  fd = socket();
  if (fd < 0)
  {
     override_state(fd, "null");   
     return 1;
  }
  close(fd);
}
```

## `flow_non_used_arg.c`

Covers flow analysis behavior for non used arg.

<!-- runnable -->

```c
#pragma safety enable

//warning: object pointed by '?' was not released.

void free(void* _Owner)
{

} //lint 29
```

## `flow_not_expr.c`

Covers flow analysis behavior for not expr.

<!-- runnable -->

```c
#pragma safety enable

char* _Opt f();

void g()
{
    const char* _Opt s1 = f();    
    if (!s1)
    {
        assert_state(s1, "null");        
    }
    else
    {        
        assert_state(s1, "not-null");
    }
}
```

## `flow_not_null_check.c`

Covers flow analysis behavior for not null check.

<!-- runnable -->

```c
#pragma safety enable

char* _Opt f();

void g()
{
    const char* _Opt s1 = f();    
    if (s1 != 0)
    {
        assert_state(s1, "not-null");        
    }
    else
    {        
        assert_state(s1, "null");
    }
}
```

## `flow_null_check.c`

Covers flow analysis behavior for null check.

<!-- runnable -->

```c
#pragma safety enable

char* _Opt f();

void g()
{
    const char* _Opt s1 = f();    
    if (s1 == 0)
    {
        assert_state(s1, "null");        
    }
    else
    {        
        assert_state(s1, "not-null");
    }
}
```

## `flow_null_deref2.c`

Covers flow analysis behavior for null deref2.

<!-- runnable -->

```c
#pragma safety enable

struct X {
    int i;
};

void f(struct X* _Opt p)
{
    int i = p ? p->i : 0; //no warning

    int i3 = p->i;      //lint 33 warning: pointer may be null

    int i2 = p ? 0 : p->i; //lint 33 warning: pointer may be null
}
```

## `flow_null_deref3.c`

Covers flow analysis behavior for null deref3.

<!-- runnable -->

```c
#pragma safety enable


int main()
{
    int* _Opt ptr = 0;

    int k = *ptr; //lint 33 warning: dereference a NULL object
    k = 0;
    return 0;
}
```

## `flow_null_else.c`

Covers flow analysis behavior for null else.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner malloc(unsigned long size);
void free(void* _Owner ptr);

void f5()
{
    void * _Owner p = malloc(1);
    
    if (p) {
       free(p);
       return;
    }
    
    assert_state(p, "null ");    
}
```

## `flow_nullable1.c`

Covers flow analysis behavior for nullable1.

<!-- runnable -->

```c
#pragma safety enable



int* _Owner make1();
int* _Owner _Opt make2();
void free(void * _Owner _Opt p);


void f(int condition)
{
  int * _Owner _Opt p = 0;
  assert_state(p, "null");

  if (condition)
  {
       assert_state(p, "null");
       p = make1();
       assert_state(p, "not-null ");
       free(p);
       p = make2();
       assert_state(p, "null | not-null ");
  }
  else
  {
    assert_state(p, "null");
  }
  free(p);
}
```

## `flow_nullable_arg.c`

Covers flow analysis behavior for nullable arg.

<!-- runnable -->

```c
#pragma safety enable


void f(int * _Owner _Opt p);
int main()
{
   int * _Owner _Opt p = 0;
   p = ((void *) 0); 
   f(0);
   f((void *) 0);
   f(nullptr);
}
```

## `flow_nullable_arg2.c`

Covers flow analysis behavior for nullable arg2.

<!-- runnable -->

```c
#pragma safety enable
void f(int  * _Opt p)
{
  assert_state(p, "null | not-null");
}
```

## `flow_nullable_deref.c`

Covers flow analysis behavior for nullable deref.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner _Opt malloc(int i);
void free(void* _Owner _Opt);

struct X {
    char* _Owner _Opt name;
};

int main()
{
    struct X* _Owner _Opt p = malloc(sizeof(struct X));
    if (p)
    {
        p->name = malloc(1);
    }
    else
    {
        //p->name = malloc(1);        
    }
    //warning: pointer may be null

    free(p->name); //lint 33 30

    // warning: object 'name' may not be empty

    free(p); //lint 29
}
```

## `flow_nullable_disable.c`

Covers flow analysis behavior for nullable disable.

<!-- runnable -->

```c
#pragma nullable disable
void f(int  *p)
{
  assert_state(p, "null | not-null");
}
```

## `flow_nullable_inside_if.c`

Covers flow analysis behavior for nullable inside if.

<!-- runnable -->

```c
#pragma safety enable



void* _Owner _Opt  malloc(unsigned long size);
void free(void* _Owner _Opt  ptr);

void f1()
{
    void * _Owner _Opt p = malloc(1);
    if (p) {
      assert_state(p, "not-null ");
    }

    assert_state(p, "null | not-null ");
    free(p);
}

void f2(int condition)
{
    void * _Owner _Opt  p = malloc(1);
    if (condition) {
      assert_state(p, "null | not-null ");
    }

    assert_state(p, "null | not-null ");
    override_state(p, "null");
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

    assert_state(p, "uninitialized");
}

void f4(int condition)
{
    void * _Owner _Opt  p = malloc(1);

    if (condition) {

    }
    else {
       free(p);
    }

    assert_state(p, "uninitialized  null | not-null ");
    override_state(p, "null");
}


void f5(int condition)
{
    void * _Owner _Opt  p = malloc(1);

    if (condition) {
       free(p);
    }
    else {

    }

    assert_state(p, "uninitialized null | not-null ");
    override_state(p, "null");
}

void f6(int condition)
{
    void * _Owner _Opt  p = malloc(1);

    if (p) {
       free(p);
       return;
    }

    assert_state(p, "null ");
}
```

## `flow_nullable_member.c`

Covers flow analysis behavior for nullable member.

<!-- runnable -->

```c
#pragma safety enable


struct Y {
    char* _Owner p0;
    int* _Owner p2;
    double i2;
};

struct X {
    char* _Owner text;
    int* _Owner p1;
    int i;
    struct Y* _Opt pY;
};

struct X f();
void destroy(_Dtor struct X *  x);

int main()
{
    struct X x;
    x = f();
    assert_state(x.text, "not-null ");
    assert_state(x.p1, "not-null ");
    assert_state(x.i, "zero | not-zero");
    assert_state(x.pY, "null | not-null");
    assert_state(x.pY->p0, "not-null ");    
    destroy(&x);
}
```

## `flow_nullable_pointer_init.c`

Covers flow analysis behavior for nullable pointer init.

<!-- runnable -->

```c
#pragma safety enable


void * f();
int main() {
  void * _Owner _Opt p = 0;
}
```

## `flow_nullable_to_nonnullable.c`

Covers flow analysis behavior for nullable to nonnullable.

<!-- runnable -->

```c
#pragma safety enable


int strlen(const char* s);

int main()
{
    //warning: cannot convert a null pointer constant to non-nullable pointer

    strlen(0); //lint 60

    char* _Opt p = 0;

    //flow
    //warning: passing a possible null pointer 'p' to non-nullable pointer parameter

    strlen(p); //lint 35

}
```

## `flow_opt_dtor.c`

Covers flow analysis behavior for opt dtor.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);
char* _Owner _Opt strdup(const char* s);

struct Y {
    char* _Owner _Opt text;
};

struct X {
    
    struct Y* _Owner pY;
};

void f(struct X* _Opt pX)
{
    if (pX)
    {
        free(pX->pY->text);
        pX->pY->text = strdup("a");
    }
}
```

## `flow_optional_owner_pointer.c`

Covers flow analysis behavior for optional owner pointer.

<!-- runnable -->

```c
#pragma safety enable


void f() {
    _Owner int * _Opt p = 0;
}
```

## `flow_or_operator.c`

Covers flow analysis behavior for or operator.

<!-- runnable -->

```c
#pragma safety enable

void f(int i)
{
    int j;

    if (i || j) //lint 30 warning: using a uninitialized object 'j'
    {
    }
}
```

## `flow_out.c`

Covers flow analysis behavior for out.

<!-- runnable -->

```c
#pragma safety enable


struct Y {
  char * _Owner p0;
  int * _Owner p2;
  double i2;
};

struct X {
  char * _Owner text;
  int * _Owner p1;
  int i;
  struct Y  *pY;
};

void init(_Ctor struct X * p);
void destroy(_Dtor struct X *  p);

int main() {
   struct X x;
   init(&x);

   assert_state(x.p1, "not-null ");
   assert_state(x.i, "zero | not-zero");
   assert_state(x.pY, "not-null");
   assert_state(x.pY->p0, "not-null ");
   assert_state(x.pY->p2, "not-null ");
   assert_state(x.pY->i2, "zero | not-zero");
   destroy(&x);
}
```

## `flow_out2.c`

Covers flow analysis behavior for out2.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner malloc(unsigned long size);
void free(void* _Owner ptr);

struct Y {
    char* _Owner p0;
    int* _Owner p2;
    double i2;
};

struct X {
    char* _Owner _Opt text;
    int* _Owner _Opt p1;
    int i;
    struct Y* _Opt pY;
};

void f(const struct X* p);
void destroy(_Dtor struct X *  p);

int main()
{
    struct X x = {0};
    f(&x);

    assert_state(x.p1, "null ");
    assert_state(x.i, "zero");
    assert_state(x.pY, "null ");

    destroy(&x);
}
```

## `flow_out_arg.c`

Covers flow analysis behavior for out arg.

<!-- runnable -->

```c
﻿#pragma safety enable

bool init(_Ctor int *a) {
    *a = 3;
    return true;
}

int main() {
    int a;
    if (init(&a) || a == 0) return a;
    if (init(&a) && a == 0) return a;
    return a;
}
```

## `flow_out_array.c`

Covers flow analysis behavior for out array.

<!-- runnable -->

```c
﻿#pragma safety enable

void f(_Ctor char s[2]) 
{
}

void test1()
{
    char s[2];
    f(s);
}

void f2(_Ctor char *s) 
{
    *s= '\0';
}

void test2()
{
    char s[2];
    f2(s);
}

int main()
{
}
```

## `flow_out_init.c`

Covers flow analysis behavior for out init.

<!-- runnable -->

```c
#pragma safety enable


void  free(void* _Owner p);
char* _Owner strdup(const char* s);

struct X {
    char* _Owner s;
};
void init(_Ctor struct X* px)
{
    assert_state(px, "not-null");
    assert_state(px->s, "uninitialized");
    px->s = strdup("a");
}

int main() {
    struct X x;
    init(&x);
    free(x.s);
}
```

## `flow_owner_get.c`

Covers flow analysis behavior for owner get.

<!-- runnable -->

```c
#pragma safety enable


int* _Owner  get();

void f()
{
    int* _Owner _Opt p = 0;
    p = get();
} //lint 29
```

## `flow_owner_get2.c`

Covers flow analysis behavior for owner get2.

<!-- runnable -->

```c

#pragma safety enable


int* _Owner  get();
void dtor(int* _Owner p);

void f(int a)
{
    int* _Owner _Opt p = 0;
    p = get();    
    dtor(p);    
}
```

## `flow_owner_increment.c`

Covers flow analysis behavior for owner increment.

<!-- runnable -->

```c
#pragma safety enable

struct X {
    char* _Owner _Opt name;
};

struct X* _Owner make();
void del(struct X* _Owner _Opt p);

int main()
{
    struct X* _Owner p = make();

    //error: operator ++ cannot be used in _Owner pointers

    p++; //lint 1310

    //error: operator -- cannot be used in _Owner pointers

    p--; //lint 1320

    del(p);
}
```

## `flow_owner_null_init.c`

Covers flow analysis behavior for owner null init.

<!-- runnable -->

```c
#pragma safety enable


#define NULL ((void*)0)

int main()
{
    const char * _Owner _Opt s;
    s = NULL;    
    s = 0;    
    s = nullptr;    
}
```

## `flow_owner_to_owner2.c`

Covers flow analysis behavior for owner to owner2.

<!-- runnable -->

```c
#pragma safety enable


void destroy(char* _Owner x);
char   * _Owner  get();

int main()
{
  destroy(get());
}
```

## `flow_owner_to_owner_arg.c`

Covers flow analysis behavior for owner to owner arg.

<!-- runnable -->

```c
#pragma safety enable


char * _Owner create();
void f(char * _Owner p);

int main()
{
    f(create());
}
```

## `flow_parameter_ifs.c`

Covers flow analysis behavior for parameter ifs.

<!-- runnable -->

```c
#pragma safety enable

struct X {
    int* _Opt pi;
};

int* f();
int main(struct X* _Opt pX)
{
    if (pX)
    {
        assert_state(pX, "not-null");
    }
    else
    {
        assert_state(pX, "null");
    }

    if (!pX)
    {
        assert_state(pX, "null");
    }
    else
    {
        assert_state(pX, "not-null");
    }

    if (pX && pX->pi)
    {
        assert_state(pX, "not-null");
        assert_state(pX->pi, "not-null");
    }
    else
    {
        //static_debug(pX);
        assert_state(pX, "not-null null");
        assert_state(pX->pi, "not-null null");
    }

    // warning: pointer is always non-null 

    if (pX == 0 || (pX->pi = f()) == 0) //lint 28
    {
        assert_state(pX, "null not-null");
    }
    else
    {
        //assert_state(pX, "not-null"); //FAILS only on github windows!? WTF
    }


}
```

## `flow_pointer_out.c`

Covers flow analysis behavior for pointer out.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

struct X {
    char* _Owner _Opt text;
};

void f(struct X* _Owner p1, struct X* _Ctor _Owner* p2)
{
    *p2 = p1;
}

int main()
{
    struct X* _Owner p1 = malloc(sizeof * p1);
    if (p1)
    {
        p1->text = 0;
        struct X* _Owner _Opt p2 = 0;
        f(p1, &p2);

        //warning: pointer may be null (p2)

        free(p2->text); //lint 33

        free(p2);
    }
}
```

## `flow_pointer_to_func_owner.c`

Covers flow analysis behavior for pointer to func owner.

<!-- runnable -->

```c
#pragma safety enable


void f(void (*pf)(void *_Owner p)) {}
```

## `flow_redundant_if.c`

Covers flow analysis behavior for redundant if.

<!-- runnable -->

```c

#pragma safety enable

struct Y {
    int type;
};

struct X {
    struct Y* _Opt current;
};

void f(struct X* ctx)
{
    if (ctx->current == 0)
        return;

    if (ctx->current != 0) //lint 28 warning: pointer is always non-null
    {

    }
    else
    {
        //still not null, so no warning
        ctx->current->type = 1;
    }

}
```

## `flow_return_after_free.c`

Covers flow analysis behavior for return after free.

<!-- runnable -->

```c
#pragma safety enable


char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct X {
    char* _Owner _Opt name;
};

struct X make()
{
    struct X x = {0};
    x.name = strdup("text");
    free(x.name);
    
    return x; //lint 30 warning: returning an uninitialized 'x.name' object 
}
```

## `flow_return_global_owner.c`

Covers flow analysis behavior for return global owner.

<!-- runnable -->

```c
#pragma safety enable


struct X { char  * _Owner _Opt owner_variable;   };
struct X global;
char * _Opt f() 
{
    return global.owner_variable;
}
```

## `flow_return_owner_to_non_owner.c`

Covers flow analysis behavior for return owner to non owner.

<!-- runnable -->

```c
#pragma safety enable


char* _Opt f()
{
    //char* _Owner _Opt p = 0;    
    //return p;
    return  0;
}
```

## `flow_return_owner_to_view.c`

Covers flow analysis behavior for return owner to view.

<!-- runnable -->

```c
#pragma safety enable


struct X { char  * _Owner owner_variable;   };
char * f(struct X *parameter) 
{
    return parameter->owner_variable;
}
```

## `flow_returning_struct.c`

Covers flow analysis behavior for returning struct.

<!-- runnable -->

```c
#pragma safety enable


char * _Owner strdup(const char* s);
void free(void * _Owner p);

struct X {
  char *_Owner _Opt name;
};

struct X make()
{
  struct X x = {0};
  x.name = strdup("text");  
  return x;
}
```

## `flow_returning_struct_from_func.c`

Covers flow analysis behavior for returning struct from func.

<!-- runnable -->

```c
#pragma safety enable


struct X {
  char * _Owner name;
};

struct X f();
struct X f2()
{
    return f();
}
```

## `flow_simple.c`

Covers flow analysis behavior for simple.

<!-- runnable -->

```c

#pragma safety enable


void * _Owner _Opt malloc(int i);
void free( void * _Owner _Opt  p);

struct X {
  char * _Owner name;    
};

int main() {
   struct X * _Owner _Opt p = malloc(sizeof * p);
   free(p);   
}
```

## `flow_struct_arg.c`

Covers flow analysis behavior for struct arg.

<!-- runnable -->

```c

#pragma safety enable


struct X { int i;  };
void f(struct X* p);
void f2(struct X* p);

void delete(struct X* p)
{
    assert_state(p, "not-null");
    f(p);
    assert_state(p, "not-null");
    f2(p);
}
```

## `flow_struct_assigment.c`

Covers flow analysis behavior for struct assignment.

<!-- runnable -->

```c

#pragma safety enable


struct Y { 
    int i;
};

struct X { 
    char * _Owner _Opt name;
};

int main()
{
    struct Y y1 = {};
    struct Y y2 = {};
    y1 = y2; //ok

    struct X x1 = {};
    struct X x2 = {};
    x1 = x2; //ok

}
```

## `flow_switch.c`

Covers flow analysis behavior for switch.

<!-- runnable -->

```c
#pragma safety enable



void* _Owner make();
void free( void* _Owner p);

void f(int condition)
{
    void* _Owner p = make();


    switch (condition)
    {
        case 1:
        {
            free(p);
        }
        break;
        case 2:
        {
            free(p);
        }
        break;

        default:
            free(p);
            break;
    }        
}
```

## `flow_switch2.c`

Covers flow analysis behavior for switch2.

<!-- runnable -->

```c
#pragma safety enable



void* _Owner _Opt calloc(int n ,unsigned size);

char* _Owner _Opt f(int i)
{
    char* _Owner _Opt p = calloc(1,2);
    switch (i)
    {
        case 1:
            break;
        case 2:
            break;
    }

    return p;
}
```

## `flow_switch3.c`

Covers flow analysis behavior for switch3.

<!-- runnable -->

```c
#pragma safety enable



void * _Owner _Opt calloc(int n , int i);

char* _Owner _Opt f(int i)
{
    char* _Owner _Opt p = calloc(1,2);        
    switch (i) {
        case 1: break;
    }
    return p;
}
```

## `flow_switch4.c`

Covers flow analysis behavior for switch4.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner calloc(unsigned n, unsigned size);
void free(void* _Owner ptr);

struct X {
    char* _Owner _Opt name;
};

struct X* _Owner _Opt F(int i)
{
    struct X* _Owner _Opt p1 = 0;

    switch (i)
    {
        case 1:
            struct X* _Owner p2 = calloc(1, sizeof * p2);
            if (p2)
            {
              override_state(*p2, "zero");
                p1 = p2;
            }
            break;
        case 2:
            break;
    }

    return p1;
}
```

## `flow_switch_4.c`

Covers flow analysis behavior for switch 4.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner malloc(unsigned size);
void free(void* _Owner _Opt ptr);


void f(int i)
{
  void * _Owner p1 = malloc(1);
  switch(i)
  {
      case 1:
      {
          void * _Owner p2 = malloc(1);
          free(p2);
      }
      break;

      case 2:
      {
          void * _Owner p3 = malloc(1);
            free(p3);
      }
      break;
  }

  free(p1);
  
}
```

## `flow_switch_5.c`

Covers flow analysis behavior for switch 5.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner malloc(unsigned size);
void free(void* _Owner _Opt ptr);
    

void f(int i)
{
    void* _Owner p1 = malloc(1);
    switch (i)
    {
        case 1:
            void* _Owner p2 = malloc(1);
            free(p2);
            break;

        case 3:
            void* _Owner p3 = malloc(1);
            free(p3);
            break;
    }

    free(p1);

}
```

## `flow_temp_owner.c`

Covers flow analysis behavior for temp owner.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

struct X {
    char* _Owner name;
};

int main()
{
    //warning: cannot assign a temporary owner to non-owner object. 
    struct X* _Opt p = (struct X* _Owner _Opt) malloc(1); //lint 23
}
```

## `flow_temp_owner2.c`

Covers flow analysis behavior for temp owner2.

<!-- runnable -->

```c
#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

int main()
{
    //warning: cannot assign a temporary owner to non-owner object.
    void* _Opt p = malloc(1); //lint 23

    //warning: cannot assign a non-owner to owner 
    free(p); //lint 25
}
```

## `flow_temp_struct.c`

Covers flow analysis behavior for temp struct.

<!-- runnable -->

```c
struct X { int i; };
struct X f()
{
    struct X x = {};
    return x;
}

int main()
{
    //error: lvalue required as left operand of assignment
    f().i = 1; //lint 1230
}
```

## `flow_ternary.c`

Covers flow analysis behavior for ternary.

<!-- runnable -->

```c
#pragma safety enable


void free(void * _Owner p);
struct X;
struct X f();
struct X { char * _Owner p; };
int main()
{
    struct X x = 1 ? f() : f(); 
    free(x.p);
}
```

## `flow_ternary2.c`

Covers flow analysis behavior for ternary2.

<!-- runnable -->

```c
#pragma safety enable



struct X {
    char * _Owner _Opt text;
};

void f(int condition) {
    struct X x1 = {};
    struct X x2 = {};
    struct X * p = condition ? &x1 : &x2;

    static_debug(p);
}
```

## `flow_trow.c`

Covers flow analysis behavior for throw.

<!-- runnable -->

```c
#pragma safety enable
int f(int condition)
{
    int * _Owner _Opt p = 0;
    try
    {
        int * _Owner _Opt p2 = p;

        //p is still null here because null pointers are not moved.
        assert_state(p, "null");

        if (condition) throw;
        p = 0;
    }
    catch
    {
    }
    
    assert_state(p, "null");
}
```

## `flow_try_catch_leak.c`

Covers flow analysis behavior for try catch leak.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner _Opt malloc(int i);
void free(void* _Owner _Opt p);
int rand();

int main()
{
    char* _Owner _Opt s = malloc(1);
    try
    {
        if (rand())
        {
            free(s);
        }
        else
        {
            //static_debug(s);
            throw;
        }
    }
    catch
    {
    }
} //lint 29 leak
```

## `flow_try_catch_while.c`

Covers flow analysis behavior for try catch while.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner malloc(unsigned size);
void free(void* _Owner ptr);

struct X {
    char* _Owner name;
};

struct X* _Owner _Opt F(int i)
{

    struct X* _Owner _Opt p1 = 0;
    try
    {
        if (i == 1)
        {
            p1 = malloc(sizeof * p1);
            while (0) {}
        }
        else if (i == 3)
        {
            p1 = malloc(sizeof * p1);
        }
    }
    catch
    {
    }

    //warning: uninitialized object 'p1.name'
    return p1;  //lint 30 30 p1.name not initialized
}
```

## `flow_unchanged_pointer.c`

Covers flow analysis behavior for unchanged pointer.

<!-- runnable -->

```c
﻿
#pragma safety enable

void* _Owner _Opt calloc(int n, unsigned long size);
void free(void* _Owner _Opt ptr);

struct Y {
    int i;
};
struct X {
    struct Y* _Owner _Opt pY;
};

void f(struct Y* p);
int main()
{

    struct X* _Owner _Opt p = calloc(1, sizeof * p);
    if (p)
    {
        p->pY = calloc(1, sizeof(struct Y));
        if (p->pY)
        {
            f(p->pY);
            p->pY->i = 1;
//          ^^^^^ still not null
        }
        free(p->pY);
        free(p);
    }
}
```

## `flow_uninitialized.c`

Covers flow analysis behavior for uninitialized.

<!-- runnable -->

```c
#pragma safety enable

void f_const(const int a);
void f(int a);


int main(void)
{
    int a;

    //warning: passing an uninitialized argument 'a' object

    f_const(a); //lint 30

    //warning: passing an uninitialized argument 'a' objec
    f(a); //lint 30 

}
```

## `flow_uninitialized_array.c`

Covers flow analysis behavior for uninitialized array.

<!-- runnable -->

```c
#pragma safety enable

void f_const(const char* s);

void f_non_const(char* s);

int main(void)
{
    char buffer[20];

    //warning: uninitialized object 'buffer' passed to non-optional parameter

    f_const(buffer); //lint 30 

    //static_debug_ex(buffer);

    //warning: uninitialized object 'buffer' passed to non-optional parameter

    f_non_const(buffer); //lint 30
}
```

## `flow_uninitialized_malloc.c`

Covers flow analysis behavior for uninitialized malloc.

<!-- runnable -->

```c
#pragma safety enable


void* _Owner _Opt malloc(int i);
void free(void* _Owner _Opt p);

struct X {
    char* _Owner name;
};

void x_delete(struct X* _Owner _Opt p)
{
    if (p)
    {
        free(p->name);
        free(p);
    }
}

int main()
{
    struct X* _Owner _Opt p = malloc(sizeof * p);

    //warning: uninitialized object 'p.name'

    x_delete(p); //lint 30
}
```

## `flow_uninitialized_struct.c`

Covers flow analysis behavior for uninitialized struct.

<!-- runnable -->

```c
#pragma safety enable

struct X {
    char* _Owner text;
};

void x_destroy(_Dtor struct X* p);

int main()
{
    struct X x;
    // warning: uninitialized object '&x.text'
    x_destroy(&x); //lint 30
}
```

## `flow_unitialized2.c`

Covers flow analysis behavior for uninitialized2.

<!-- runnable -->

```c
#pragma flow enable

void f_const(const char* s);

void f_non_const(char* s);

int main(void)
{
    char buffer[20];

    //warning: uninitialized object 'buffer' passed to non-optional parameter

    f_const(buffer); //lint 30

    f_non_const(buffer);
}
```

## `flow_use_affter_free.c`

Covers flow analysis behavior for use after free.

<!-- runnable -->

```c
#pragma safety enable


char* _Owner strdup(const char* s);
void* _Owner malloc(unsigned size);

void free(void* _Owner _Opt ptr);

struct X {
    char* _Owner _Opt name;
};

void x_destroy(_Dtor struct X* p)
{
    free(p->name);
}

void x_print(struct X* p)
{
    //printf("%s", p->name);
}

int main()
{
    //warning: object pointed by 'x.name' was not released.
    struct X x = {0};
    x.name = strdup("a");
    x_destroy(&x);

    //warning: uninitialized object '&x.name'
    x_print(&x); //lint 30

} //lint 29
```

## `flow_value.c`

Covers flow analysis behavior for value.

<!-- runnable -->

```c
#pragma safety enable



bool f()
{
    int* _Opt p = 0;

    //warning: pointer is always null

    return p == 0; //lint 28
}
```

## `flow_view_member.c`

Covers flow analysis behavior for view member.

<!-- runnable -->

```c
#pragma safety enable
 
struct X {
    char * _Owner text;
};

struct Y {
    _View struct X x;
};

struct Y f();
int main()
{
    struct Y y = f();
}
```

## `flow_view_qualifier.c`

Covers flow analysis behavior for view qualifier.

<!-- runnable -->

```c
#pragma safety enable


struct X {
  char * _Owner name;
};

_View struct X f();

int main()
{
  f();
}
```

## `flow_void_ptr_leak.c`

Covers flow analysis behavior for void ptr leak.

<!-- runnable -->

```c
#pragma safety enable


struct X { char* _Owner text; };

void x_destroy(_Dtor struct X* p);

void x_delete(struct X* _Owner _Opt p )
{
    if (p)
    {
        x_destroy(p);
        //memory pointed by p not deleted
    }
} //lint 29
```

## `flow_while2.c`

Covers flow analysis behavior for while2.

<!-- runnable -->

```c
#pragma safety enable

void f(char* p)
{   
  while (*p) { p++; }
}
```
