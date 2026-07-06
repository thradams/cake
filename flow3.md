# Flow3

## Functions

### Sample 1

<!-- runnable -->

```c
#pragma nullable enable

char* _Opt make();
void receives_non_null(char* p);

int main()
{
    char* _Opt p1 = make();
    receives_non_null(p1);
}

```


### Sample 2

<!-- runnable -->

```c
#pragma nullable enable

void receives_non_null(char* p);

int main()
{
    receives_non_null(0);
}

```

### Sample 3

<!-- runnable -->

```c
#pragma nullable enable

struct X { char* name; };

void receives_non_null(struct X* p) {};

int main()
{
    struct X x = {};
    receives_non_null(&x);
}
```

### Sample 4

<!-- runnable -->

```c
#pragma nullable enable

struct X { char* name; };

void receives_non_null(struct X* p) {};

int main()
{
    struct X x;
    receives_non_null(&x);
}
```


### Sample 5

<!-- runnable -->

```c
#pragma nullable enable

struct X { char* name; };

void f(struct X x) {};

int main()
{
    struct X x;
    f(x);
}
```

### Sample 6

<!-- runnable -->

```c
#pragma nullable enable

struct X { char* name; };

struct X return_struct()
{
    struct X x;
    return x;
}
```

### Sample 7

<!-- runnable -->

```c
#pragma nullable enable

char* _Opt make();

char * returns_non_null()
{
    char* _Opt p1 = make();
    return p1;
}

```


### Sample 8

<!-- runnable -->

```c
#pragma nullable enable

struct X { char* name; };

struct X return_struct()
{
    struct X* p;
    {
        struct X x;
        p = &x;
    }
    static_debug(*p);
    return *p;
}
```



### Sample 9

<!-- runnable -->

```c

#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);

int * _Owner _Opt f(int c) {
    int * _Owner _Opt p = malloc(sizeof *p);
    return p;
}

```

### Sample 10

<!-- runnable -->

```c
#pragma nullable enable

struct X { char* name; };

struct X * f()
{
    struct X* p;
    {
        struct X x;
        p = &x;
    }
    
    return p;
}

```


### Sample 11

<!-- runnable -->

```c
#pragma safety  enable


void f(int* _Owner p2)
{
    int* _Owner p1 = p2;
}
```

### Sample 12

<!-- runnable -->

```c
#pragma safety  enable


void f(int* _Owner p2)
{
    int* _Owner p1;
    p1 = p2;
}
```



### Sample 13

<!-- runnable -->

```c
#pragma safety  enable

void consume(int* _Owner p);

void f(int* _Owner p2)
{
    int* _Owner p1 = p2;
    consume(p1);
}
```

### Sample 14

<!-- runnable -->


```c
#pragma safety  enable

void consume(int* _Owner p);

void f(int* _Owner p2)
{
    int* _Owner p1;
    p1 = p2;
    consume(p1);
    static_debug(*p1); //ended
}

```

### Sample 15

<!-- runnable -->

```c

#pragma nullable enable

struct X { int i; int j; };

void clear(_Clear struct X* p) {};

int main()
{
    struct X x = {1, 2};
    clear(&x);
    static_debug(x); //should print  0
}

```


### Sample 16

<!-- runnable -->

```c

#pragma nullable enable

void f(int i) {

  int a;
     switch(i)
     {
        case 1:
          a = 1;
        break;

        case 2:
          a = 2;
        break;
        
        default:
          a = 3;
     }
     static_debug(a);
};


```

### Sample 17

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


### Sample 18

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


### Sample 19

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
        compile_assert(p1 != 0);
        compile_assert(p2 != 0);
    }
    else
    {
    }
}
```


### Sample 20

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
        compile_assert(s1 != 0);
    }
    else
    {        
        //assert_state(s1, "null not-null");
    }
}
```


### Sample 21

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


### Sample 22

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


### Sample 23

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

### Sample 24

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

### Sample 25

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

### Sample 26

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
        compile_assert(p != 0);
    }
    else
    {
        compile_assert(p != 0);
    }
}
```

### Sample 27

<!-- runnable -->

```c
struct X { int i; void* p; };
int main() {
	struct X x;
	x = (struct X){ 0 };
	compile_assert(x.i == 0);
	compile_assert(x.p == 0);
}
```

### Sample 28

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
 
   compile_assert(x->p1 == 0);
   compile_assert(x->i == 0);
   compile_assert(x->pY == 0);
   free(x);
}
```

### Sample 29

<!-- runnable -->

```c
#pragma safety enable



struct X { int i; void* _Opt p; };
void* _Owner _Opt calloc(int i, int sz);
void free(void* _Owner _Opt p);

int main()
{
    struct X* _Owner _Opt p = calloc(1, 1);
    if (p)
    {
    compile_assert(p->i == 0);
    compile_assert(p->p == 0);
    }
    free(p);
}
```

### Sample 30

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

### Sample 31

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

### Sample 32

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
    compile_assert(x.p == 0);
}
```

### Sample 33

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
   compile_assert(x.text == 0);
   compile_assert(x.p1 == 0);
   compile_assert(x.i == 0);
   compile_assert(x.pY == 0);
}
```

### Sample 34

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

### Sample 35

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

### Sample 36

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

### Sample 37

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

### Sample 38

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

### Sample 39

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

### Sample 40

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

### Sample 41

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

### Sample 42

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
   compile_assert(x.text == 0);
   compile_assert(x.p1 == 0);
   compile_assert(x.i == 0);
   compile_assert(x.pY == 0);
}
```

### Sample 43

<!-- runnable -->

```c
#pragma safety enable

int g = 1;

void f1()
{
     //externa non constant variable are any state
     g  = 1;
     compile_assert(g != 0);
     g = 0;
     compile_assert(g == 0);
}

void f2()
{
     g = 2;
     g = 3;
}
```

### Sample 44

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

### Sample 45

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
    for (; p; p = p->next)
    {
        compile_assert(p != 0);
    }
    compile_assert(p == 0);

    for (p = get(); p; p = p->next)
    {
        compile_assert(p != 0);
    }
    compile_assert(p == 0);

    for (p = get(); p; )
    {
        compile_assert(p != 0);
        p = p->next;
    }
    compile_assert(p == 0);

    for (p = get(); p; )
    {
        if (c)
        {
            compile_assert(p != 0);
            break;
        }
        p = p->next;
    }
   
    for (struct X* _Opt p2 = get(); p2; )
    {
        compile_assert(p2 != 0);
        f(p2);
    }

    
    struct X* _Opt p3 = get();
    for (;;)
    {
        break;
    }
    
}
```

### Sample 46

<!-- runnable -->

```c
int main()
{
  int j;
  for (j = 0; j <10; j++) {}
  return j;
}
```

### Sample 47

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

### Sample 48

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

### Sample 49

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

### Sample 50

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

### Sample 51

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

### Sample 52

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

### Sample 53

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

### Sample 54

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
    free(p1);
    return 0;
}
```

### Sample 55

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

### Sample 56

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
        compile_assert(i != 0);
    }
   
}
```

### Sample 57

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

### Sample 58

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

### Sample 59

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

### Sample 60

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

### Sample 61

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

### Sample 62

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

### Sample 63

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

### Sample 64

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

### Sample 65

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

### Sample 66

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

### Sample 67

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

### Sample 68

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

### Sample 69

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

### Sample 70

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

### Sample 71

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

### Sample 72

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

### Sample 73

<!-- runnable -->

```c
int main()
{
    int* p = &1; //lint 1220 error: lvalue required as unary '&' operand 
}
```

### Sample 74

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
  
  // assert_state(x->p1, "uninitialized");
   //assert_state(x->i, "uninitialized");
   //assert_state(x->pY, "uninitialized");
   free(x);
}
```

### Sample 75

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
      if (p)
    {
    }
    free(p);
}
```

### Sample 76

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

### Sample 77

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

### Sample 78

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

### Sample 79

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

### Sample 80

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

### Sample 81

<!-- runnable -->

```c
#pragma safety enable


char * _Owner _Opt f() {
    char * _Owner _Opt p = 0;
    return p; /*implicit move*/
}
```

### Sample 82

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

### Sample 83

<!-- runnable -->

```c
#pragma safety enable

void free(void *_Owner _Opt p);

void f(int* _Opt _Owner p)
{
   int * _Opt _Owner p2 = p;   
   //assert_state(p, "null, moved");
   if (p)
   {
     //assert_state(p, "moved");
   }
   free(p2);
}
```

### Sample 84

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

### Sample 85

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

### Sample 86

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

### Sample 87

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

### Sample 88

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

### Sample 89

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
    
    compile_assert(s != 0);
    free(s);
}
```

### Sample 90

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

### Sample 91

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
    compile_assert(x.p == 0);
    f(&x);
    //assert_state(x.p, "null | not-null");
    free(x.p);
}
```

### Sample 92

<!-- runnable -->

```c
#pragma safety enable

void f(int *p)
{
    compile_assert(p != 0);
}
```

### Sample 93

<!-- runnable -->

```c
#pragma safety enable
void f(int  *p)
{
  compile_assert(p != 0);
}
```

### Sample 94

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

### Sample 95

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

### Sample 96

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

### Sample 97

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

### Sample 98

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

### Sample 99

<!-- runnable -->

```c
#pragma safety enable

//warning: object pointed by '?' was not released.

void free(void* _Owner)
{

} //lint 29
```

### Sample 100

<!-- runnable -->

```c
#pragma safety enable

char* _Opt f();

void g()
{
    const char* _Opt s1 = f();    
    if (!s1)
    {
        compile_assert(s1 == 0);
    }
    else
    {        
        compile_assert(s1 != 0);
    }
}
```

### Sample 101

<!-- runnable -->

```c
#pragma safety enable

char* _Opt f();

void g()
{
    const char* _Opt s1 = f();    
    if (s1 != 0)
    {
        compile_assert(s1 != 0);
    }
    else
    {        
        compile_assert(s1 == 0);
    }
}
```

### Sample 102

<!-- runnable -->

```c
#pragma safety enable

char* _Opt f();

void g()
{
    const char* _Opt s1 = f();    
    if (s1 == 0)
    {
        compile_assert(s1 == 0);
    }
    else
    {        
        compile_assert(s1 != 0);
    }
}
```

### Sample 103

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

### Sample 104

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

### Sample 105

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
    
    compile_assert(p == 0);
}
```

### Sample 106

<!-- runnable -->

```c
#pragma safety enable



int* _Owner make1();
int* _Owner _Opt make2();
void free(void * _Owner _Opt p);


void f(int condition)
{
  int * _Owner _Opt p = 0;
  compile_assert(p == 0);

  if (condition)
  {
       compile_assert(p == 0);
       p = make1();
       compile_assert(p != 0);
       free(p);
       p = make2();
       //assert_state(p, "null | not-null ");
  }
  else
  {
    compile_assert(p == 0);
  }
  free(p);
}
```

### Sample 107

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

### Sample 108

<!-- runnable -->

```c
#pragma safety enable
void f(int  * _Opt p)
{
  //assert_state(p, "null | not-null");
}
```

### Sample 109

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

### Sample 110

<!-- runnable -->

```c
#pragma nullable disable
void f(int  *p)
{
  //assert_state(p, "null | not-null");
}
```

### Sample 111

<!-- runnable -->

```c
#pragma safety enable



void* _Owner _Opt  malloc(unsigned long size);
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
    void * _Owner _Opt  p = malloc(1);
    if (condition) {
      //assert_state(p, "null | not-null ");
    }

    //assert_state(p, "null | not-null ");
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

    //assert_state(p, "uninitialized");
}

void f4(int condition)
{
    void * _Owner _Opt  p = malloc(1);

    if (condition) {

    }
    else {
       free(p);
    }

    //assert_state(p, "uninitialized  null | not-null ");
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

    //assert_state(p, "uninitialized null | not-null ");
    override_state(p, "null");
}

void f6(int condition)
{
    void * _Owner _Opt  p = malloc(1);

    if (p) {
       free(p);
       return;
    }

    compile_assert(p == 0);
}
```

### Sample 112

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
    compile_assert(x.text != 0);
    compile_assert(x.p1 != 0);
    //assert_state(x.i, "zero | not-zero");
    //assert_state(x.pY, "null | not-null");
    compile_assert(x.pY->p0 != 0);
    destroy(&x);
}
```

### Sample 113

<!-- runnable -->

```c
#pragma safety enable


void * f();
int main() {
  void * _Owner _Opt p = 0;
}
```

### Sample 114

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

### Sample 115

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

### Sample 116

<!-- runnable -->

```c
#pragma safety enable


void f() {
    _Owner int * _Opt p = 0;
}
```

### Sample 117

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

### Sample 118

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

   compile_assert(x.p1 != 0);
   //assert_state(x.i, "zero | not-zero");
   compile_assert(x.pY != 0);
   compile_assert(x.pY->p0 != 0);
   compile_assert(x.pY->p2 != 0);
   //assert_state(x.pY->i2, "zero | not-zero");
   destroy(&x);
}
```

### Sample 119

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

    compile_assert(x.p1 == 0);
    compile_assert(x.i == 0);
    compile_assert(x.pY == 0);

    destroy(&x);
}
```

### Sample 120

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

### Sample 121

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

### Sample 122

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
    compile_assert(px != 0);
    //assert_state(px->s, "uninitialized");
    px->s = strdup("a");
}

int main() {
    struct X x;
    init(&x);
    free(x.s);
}
```

### Sample 123

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

### Sample 124

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

### Sample 125

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

### Sample 126

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

### Sample 127

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

### Sample 128

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

### Sample 129

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
        compile_assert(pX != 0);
    }
    else
    {
        compile_assert(pX == 0);
    }

    if (!pX)
    {
        compile_assert(pX == 0);
    }
    else
    {
        compile_assert(pX != 0);
    }

    if (pX && pX->pi)
    {
        compile_assert(pX != 0);
        compile_assert(pX->pi != 0);
    }
    else
    {
        //static_debug(pX);
        //assert_state(pX, "not-null null");
        //assert_state(pX->pi, "not-null null");
    }

    // warning: pointer is always non-null 

    if (pX == 0 || (pX->pi = f()) == 0) //lint 28
    {
        //assert_state(pX, "null not-null");
    }
    else
    {
        //assert_state(pX, "not-null"); //FAILS only on github windows!? WTF
    }


}
```

### Sample 130

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

### Sample 131

<!-- runnable -->

```c
#pragma safety enable


void f(void (*pf)(void *_Owner p)) {}
```

### Sample 132

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

### Sample 133

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

### Sample 134

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

### Sample 135

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

### Sample 136

<!-- runnable -->

```c
#pragma safety enable


struct X { char  * _Owner owner_variable;   };
char * f(struct X *parameter) 
{
    return parameter->owner_variable;
}
```

### Sample 137

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

### Sample 138

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

### Sample 139

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

### Sample 140

<!-- runnable -->

```c

#pragma safety enable


struct X { int i;  };
void f(struct X* p);
void f2(struct X* p);

void delete(struct X* p)
{
    compile_assert(p != 0);
    f(p);
    compile_assert(p != 0);
    f2(p);
}
```

### Sample 141

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

### Sample 142

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

### Sample 143

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

### Sample 144

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

### Sample 145

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

### Sample 146

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

### Sample 147

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

### Sample 148

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

### Sample 149

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

### Sample 150

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

### Sample 151

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

### Sample 152

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

### Sample 153

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
        compile_assert(p == 0);

        if (condition) throw;
        p = 0;
    }
    catch
    {
    }
    
    compile_assert(p == 0);
}
```

### Sample 154

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

### Sample 155

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

### Sample 156

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

### Sample 157

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

### Sample 158

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

### Sample 159

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

### Sample 160

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

### Sample 161

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

### Sample 162

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

### Sample 163

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

### Sample 164

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

### Sample 165

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

### Sample 166

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

### Sample 167

<!-- runnable -->

```c
#pragma safety enable

void f(char* p)
{   
  while (*p) { p++; }
}
```
