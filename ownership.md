
# Ownership checks for C

> This feature provides the experience of engaging in pair programming with an attentive developer who ensures program safety.

## One minute tour

In the context of this feature, a _destructor_ is a function called before an object's lifetime ends. The _lifetime_ of an object refers to the period during which it is accessible and valid.

Introducing the new qualifier _owner_, which can be used to declare a type that requires the invocation of a _destructor_ before the end of its lifetime.

For example, we can declare `malloc` as follows:


```c
void * owner malloc(size_t sz);
```

To assign ownership to the result of `malloc`, we need to assign it to an _owner_ variable:

```c
int main() {
  void * owner p = malloc(1);
}
```

The compiler will generate an error message stating that the object 'p' was not moved or destroyed.

To address this, we define a destructor for the object by declaring `free` as follows:

```c
void free(void * owner p);
```
 
```c
int main() {
  void * owner p = malloc(1);
  free(p); //p cannot be moved to free implicitly
}
```

The compiler informs us that 'p' cannot be moved to `free` implicitly and suggests using `move` before the argument.

By explicitly using `move`, we indicate that the variable has been moved or destroyed at the caller's side.

```
   free(move p);
```

The reason for that I want to make clear at the caller side that a variable has been moved/destroyed. 

However, for certain functions with obvious move semantics based on their names, we can use the `[[implicit]]` attribute to make the usage of `move` optional:

```c
void free([[implicit]] void * owner p);
```


I hope this brief tour provides you with a glimpse of what I aim to achieve.

## Move assignment

With the introduction of the _owner_ qualifier, certain changes in the type system are necessary. Similar to being cautious and explicit when moving a variable into a function, we should adopt the same approach for assignments.

Consider this sample

```c
int main() {
  void * p1 = malloc(1);
  void * p2 = malloc(1);
  p1 = p2;
  free(p1);
  free(p2);
}
```

In this case, assigning `p2` into `p1` leads to a memory leak of the `p1` object and a double free of the `p2` object.

To address this issue, the compiler will check the end of lifetime of `p1` before the assignment.  

The move assignment can be only used if both variables are owner.
  
The syntax is:

```
p1 = move p2;
```

There is nothing especial on this assignment compared with the normal assignment. The only difference is that the intention is explicit. 

After any move (assignment or function argument), the object transitions into an uninitialized state. This state is only for static analysis purposes and has no runtime implications.

Returning to our sample:

```c
int main() {
  void * owner p1 = malloc(1);
  void * owner p2 = malloc(1);
  
  //error p1 was not moved or destroyed
  p1 = move p2;
  
  free(p1);
  free(p2);
}
```

The compiler will complain that `p1` was not moved or destroyed before the assignment. To resolve this, we modify the code as follows:

```c
int main() {
  void * owner p1 = malloc(1);
  void * owner p2 = move p1;
  free(p2);
}
```


If you attempt to use `p1` after moving it, the compiler will issue a warning about using an uninitialized variable.

In some cases, it may not be possible to determine if an object is initialized or not. For such scenarios, the compiler suggests using options like assertions, the `[[uninitialized]]` attribute, or destroying the object before assignment.  

For instance:

```c
struct X { char * owner name; };
void some_function(struct X * p) {
  //error: unknown p->name state
  p->name = strdup("new text");
}
```

In this case, the compiler will display a message stating that the state of `p->name` cannot be determined and provides suggestions for handling this situation:

This is how to fix it

```c
  free(p->name);
  p->name = strdup("new text");
```
or
```c
  assert(p->name == NULL);
  p->name = strdup("new text");
```
or
```c
  [[unitialized]] p->name = move strdup("new text");
```


We can assign a non owner to a owner.

```c
  a = b; /* b is owner*/
```

In this situation the ownership is not transfered. The same think happens when we can function arguments that are not owners.

```c
  void F(T a) {}
  owner T a;
  F(a); /*ownership is not transfered*/
```

## structs/union/enum

We can apply the _owner_ qualifier to structs, unions, and enums as well:

```c
int main() {
 owner struct X x = {};
}
```

This syntax works. However, if we forget to include the qualifier for an object that requires a destructor, we have a leak.

To address this, an additional syntax is provided for tagged objects:

```c
struct owner X {
   ...
}
```

By using this syntax, the object is qualified as _owner_ by default:

```c
int main() {
   struct X x = {};    
} //**"object 'x' was not moved/destroyed"**
```


## pointers

So far, the pointer samples have used `void *`. Now, let's consider the following situation:

```c  
struct owner X { char * owner name; };
int main() {
   struct X * owner p = new_x();    
}
```

Here, `p` is an _owner_ pointer to an _owner_ object. In this case, the pointer is the owner of both the memory and the object. Moving the pointer will transfer both responsibilities, resulting in the destruction of the object and the memory.

Consider this sample

```c  

void x_delete([[implicit]] struct X * owner p) 
{
   if (p){
      p->free(name);
      free(p);
   }
}
int main() {
   struct X * owner p = new_x();
   x_delete(p);   
} 
```

This code demonstrates the same behavior as the following code:


```c  
int main() {
   struct X * owner p = new_x();
   if (p){
      p->free(name);
      free(p);
   }   
} 
```

Both scenarios require the same checks, indicating that there is nothing special about the destructors. 

The compiler needs to check each _owner_ member of the struct individually:


```c  
int main() {
   struct X * owner p = new_x();
   if (p){
      free(p->name);
      free(p);
   }   
} 
```

Having this logic in a specialized function makes the compiler's job easier, as the flow analysis becomes simpler. It's important to assist the compiler in order to leverage its capabilities.

Another detail to note is that when we free an _owner_ pointer using `void *`, the compiler assumes that we are destroying the memory and not the pointed object. 

If the pointed object is also an _owner_, the compiler checks if the object is destroyed first. In the provided sample, `free(p->name);` was the only _owner_ member of the struct, so it was safe to call `free` on `p`.


## destroying structs\unions 

Consider:

```
struct owner X { char * owner name; };
void x_destroy([[implicit]] struct X x) 
{
   free(x.name);
}
int main() {
   struct X x;
   x_destroy(x);   
} 
```

This code is correct and works as expected.   
  
However, if we want to pass the struct using a pointer like:


```
void x_destroy([[implicit]] struct X * owner p) {
   free(p->name);
}
```

The problem arises when we pass an _owner_ pointer. The compiler assumes that we want to destroy both the object and the memory. However, in this case, the object is on the stack, and we only want to destroy the object, not to free the memory.

To address this, a qualifier called `obj_owner` is introduced, which can only be used for pointers:

```
void x_destroy([[implicit]] struct X * obj_owner p) {
   free(p->name);
}
```

This qualifier indicates that the pointer is the owner of the object but not the owner of the memory.


## Returning owner type

Returning an _owner_ variable is the same as moving it. The design decision here was not require the `move` keyword.


```c
struct list make()
{
  struct list {...};
  return list; /*moved*/
}
```

## Owner arrays
As expected arrays and pointer are related.

The _owner_ qualifier can be placed inside the array together with the array size:

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

We can also pass an _owner_ pointer:

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

By convention, passing an _owner_ pointer to an array destructor will not transfer ownership of the memory, just of the pointed object.

To destroy both the array and the memory, we can use:

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

## Reality check I

Let's examine how these rules can help with `fopen` and `fclose`.


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

In this scenario, we encounter a problem because not all control paths call the destructor. The compiler would emit a warning in such cases.

However, the code is correct because we don't need to and cannot call `fclose` on a null pointer.

To address this, null checks need to be implemented in the static analyzer. The compiler will not emit warnings if it can prove that an _owner_ variable is empty or uninitialized at the end of its lifetime.


## Reality check II

```c
int main()
{
  FILE * owner f = NULL;
  if (fopen_s( &f,"f.txt", "r") == 0)  {
    fclose([[initialized]] f);
  }
  [[uninitialized]] f;
}
```

When initialization needs to be checked using a result code, we don't have semantics to provide the necessary information to the compiler. In this case, an annotation `

In this case, an annotation `[[initialized]]` is needed to inform the compiler that the variable is initialized, and an annotation `[[uninitialized]]` is needed to inform the compiler that the variable is uninitialized.


## What's next?
Implement this in cake.
http://thradams.com/cake/index.html
The hard part is the flow analysis.

## Conclusion
This feature aims to provide ownership checks in C by introducing the _owner_ qualifier. It ensures that objects are properly destroyed or moved before their lifetime ends, preventing memory leaks and use-after-free errors. The compiler assists in detecting potential issues and suggests necessary changes to the code. By leveraging ownership checks, developers can write safer and more reliable code in C.

## Motivation

I have placed the motivation section at the end, considering that memory safety guarantees are already a widely discussed topic and the motivation becomes apparent as readers delve into the content.

In the C programming language, manual management of resources such as memory is necessary. We rely on functions like `malloc` to allocate memory and store the resulting address in a variable. To properly deallocate memory when it is no longer needed, we must use the address returned by `malloc` and call the `free` function.

Consequently, the variable holding the memory address is considered the owner of that memory. Discarding this address without calling `free` would result in a memory leak, which is an undesirable scenario.

Resource leaks present a significant challenge because they often remain silent problems, initially having no immediate impact on a program's behavior or causing immediate issues. These leaks can easily go unnoticed during unit tests, creating a false sense of security. It is crucial to address and track these problems early on. By doing so, we can not only prevent potential complications but also save valuable time and resources in the long run.

Moreover, these checks also help prevent occurrences of double free or use-after-free issues. While both problems typically lead to immediate failures at runtime, having preventive measures in place is highly advantageous.

