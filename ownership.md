
V0.4 - 01/07/2023
# Ownership checks for C

> **cake compiler** will make you feel like you're engaging in pair programming with an experienced and attentive developer working to make the program safe.


## One minute tour

To inform the compiler a type must be moved/destroyed, we add `owner`.

```c
void * owner malloc(int 1);
```

When a non owner type is assigned to a function returning a owner, the compiler will complain.

```c
void * owner malloc(int 1);
int main() {
  void * p = malloc(1);
  //missing owner qualifier in 'p'
}
```

After adding the owner qualifier we have this warning.

```
int main() {
  void * owner p = malloc(1);
} //warning: owner variable 'p' not moved/destroyed
```

To destroy p we can make

```c
void free(void * owner p);
int main() {
  void * owner p = malloc(1);
  free(move p);
}
```

When we move a variable we want to be explicit about it. That is the reason of using keyword move in function arguments. 

However some functions names make this so obvious that we can just ignore move adding `[[implicit]]` 
```c
void free([[implicit]] void * owner p);
...
free(p);
```

## Syntax
  
We have a new qualifier called **owner**. When this qualifier is used, the variable needs to be moved or destroyed before the end of it's lifetime.

The syntax is similar of const qualifier.

```c
owner int handle; 
void * owner ptr = malloc(1);
int arr[owner 10];
owner struct X x;
```

For structs it can be placed before the tag, this will make local variables of this struct automatically owner.

```c
struct owner X {  
  resource rc;   
};
int main(){
  struct X x; //same as owner struct X x;
}
```


## Owner pointer checks

When we have owner pointer to a owner object, the compiler will check if both, pointer and pointed object are moved/destroyed before the end of scope.  

```c
owner struct X { ... };
int main() {
 struct X * owner p = calloc(1, sizeof(*p));
 x_destroy(p);
 free(p);
}//ok
```


## Move assignment

The _move assignment_ also *move initialization* is used to transfer the ownership of some owner variable to another owner variable.

For the variable receiving the value, we have a situation similar of leaving the scope. Compiler must check if the variable is uninitialized or null before receiving the new value.

After the _move assignment_ / *move initialization* the source variable became uninitialized. The uninitialized is just conceptual state, nothing changes at runtime.

```c
void * owner p1 = malloc(1);
void * owner p2 = nullptr;
p2 = move p1;
free(p2);
```

If we try to use p1 after move, we receive a warning "using a uninitialized variable".

We can assign a owner variable to a non owned variable.
In this case we have a view only.

## Moving to function arguments

Passing a variable to a function is very similar of assignment. The same rules apply.

```c
owner struct list {...};
owner struct node {...};

struct list list = {0};
struct node node = {0}
...
list_add(&list, move node);
```

## Implicit move

Some moves are so obvious that we can make them optional. For instance, free, close, destroy and delete are good candidates.
For this job we can use the attribute [[implicit]].

```c
void list_destroy([[implicit]] struct list l) { }

int main()
{
  owner struct list {...};
  list_destroy(list);
}
```

## Returning owner type
Returning a owner variable is the same of moving it and it is implicit.

```c
struct list make()
{
  struct list {...};
  return list;
}
```

## obj_owner qualifier

We can create destructors like this

```c
void x_destroy([[implicit]] struct list list) {
}

int main()
{
  struct list list = {};
  x_destroy(list);
}
```

But we may want to create destructor passing pointers.
The problem is that a owner pointer is owner of both memory and the object but for objects allocated on the stack we want to destroy only the object.

The `obj_owner` qualifier was created basically to allow destructors to be implemented as:

```c
void x_destroy([[implicit]] struct list* obj_owner list) {
}

int main()
{
  struct list list = {};
  x_destroy(&list);
}
```


## Owner arrays
As expected arrays and pointer are related. However, a parameter of owner array type is equivalent of `obj_owner`.  

```c
void array_destroy(int n, struct X a[owner n])
{
}

int main()
{
  struct X a[owner 100];
  array_destroy(100, a);
}
```

We can pass owner pointer to onwer array parameters, but then we also need to free.

```c
void array_destroy(int n, struct X a[owner n])
{
}

int main()
{
  struct X * owner p = calloc(100, sizeof(struct X));
  array_destroy(100, p);
  free(p);
}
```


We also can use owner pointers

```c
void array_delete(int n, struct X * owner p)
{
}

int main()
{
  struct X * owner p = calloc(100, sizeof(struct X));
  array_delete(100, p);  
}
```

## Checks inside destructors
When we move a owner variable, we postponed the checks. But the checks must be done somewhere, for instance inside the destructor.

Let's assume you want just ignore the checks inside the destructor.

For this task we can just use [[ignore]].

```c  
[[ignore]] variable;
```

## Reality check I

Let's check if these rules can help us with ```fopen/fclose```.

```c 
FILE* owner fopen(char const* name,char const* mode);
int fclose([[implicit]] FILE* owner f);
```

```c
int main() {
  FILE * owner p = fopen("text.txt", "r");
  if (p) {
    fclose(p);
  }
}
```

We have a problem, because the not all control paths are calling the destructor and the compiler will emit an warning.

However, the code is correct because we don't need, and we cannot, call fclose on null pointer.

To solve this problem we also need null-checks in your static analyzer.  
  
The compiler will not emit warning if it can prove that a owner variable is empty or uninitialized at the end of scope. 

## Reality check II

```c
onwer struct book {
    char * owner title;
};

void book_destroy(struct book * obj_owner book) { 
  free(book->title);
}

void book_delete([[implicit]] struct book* owner book) {
    if (book) {
       book_destroy(book);
       free(book);
    }
}
owner struct books {
    struct book * owner * owner data;
    int size;
    int capacity;
};

void books_destroy([[implicit]] struct books * obj_owner books) 
{
   for (int i = 0; i < books->size; i++) {
     book_delete(books->data[i]);
   }
   free(books->data);
}

int books_push_back(struct books* p, 
                    struct book* owner book)
{
    //... 
    p->data[p->size] = move book;
    //...
}

int main() 
{
   struct books books = {};
   struct book* owner book = calloc(1, sizeof (struct book));
   if (book == NULL) goto continuation;
   
   book->title = strdup("book1");
   if (book->title == NULL) goto continuation;
   
   if (books_push_back(&books, move book) == 0) {
     book = NULL;
   }
 continuation:
  book_delete(book);
  books_destroy(&books);
}
```


## Reality check III

```c
int main()
{
  FILE * owner f = NULL;
  if (fopen_s( &f,"f.txt", "r") == 0)  {
    fclose(f);
  }
}
```

The problem here is that in previous fopen we could check for null to decide if we need or not a warning if the destructor is not called.


## Reality check IV

```c
struct X {
  char * owner text;
};
const char * f(struct X * p) {
  return p->text; //returning owner to non owner is OK
}
```

## Reality check V

```c
struct X {
  char * owner text;
};

const char * _owner f() {
  struct X x;
  return x.text; //moved
}
```




# What's next?
Implement this in cake!

## Motivation

In C, resources such as memory are managed manually. For example, we utilize the `malloc` function to allocate memory and store the resulting address in a variable. When the memory is no longer needed, we need the address returned by `malloc` to be able to call `free`.  

Therefore, the variable holding the address is considered the owner of the memory, as this address cannot be simply discarded, otherwise we have a memory leak.

Resource leaks pose a significant challenge as they tend to be silent problems that don't immediately impact a program's behavior or cause immediate issues. Moreover, they can easily go unnoticed during unit tests, creating a false sense of security. Therefore, it is absolutely crucial to address and track these problems early on. By doing so, not only can potential complications be prevented, but it can also save valuable time and resources in the long run.

This checks also prevent double free, or use after free.   Both problems generally fail fast in runtime, but it is also good to have.

To check the correctness of owner variables this proposal suggests new type qualifiers `owner`, `view` and `obj_owner` and a move assignment.

The `owner` qualifier will qualify the variable as the `owner` and `view` can be used to override (negate) `owner`.

Owner variables cannot be discarded, they must be _moved_ or _destroyed_.

The `obj_owner` is a special case of `owner` pointer and it will be explained later.



