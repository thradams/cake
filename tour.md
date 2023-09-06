# Checked ownership in C

In this tour, we'll explore the concept of ownership in the C programming language having the experimental ownership checks implemented by cake compiler to help us.  
  
C is a low-level language where developers have direct control over memory management, making ownership a critical aspect of writing efficient and bug-free code. Cake implements a static analysis verification to ensure the programmer is not making mistake while implementing the ownership.  

**Introduction to Ownership in C**

Ownership in C refers to the responsibility a developer has for managing allocated memory. In C, you must explicitly allocate and deallocate memory, giving you full control but also the potential for errors.

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int *dynamicArray = (int *)malloc(5 * sizeof(int)); // Allocate memory
    if (dynamicArray == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    // Use dynamicArray
    for (int i = 0; i < 5; i++) {
        dynamicArray[i] = i * 10;
    }

    // Deallocate memory
    free(dynamicArray);
    return 0;
}
```

In this example, we manually allocate memory using `malloc` and deallocate it using `free`.

**Ownership and Memory Leaks**

One common issue in C is memory leaks, which occur when allocated memory is not properly deallocated. Failing to release memory can lead to programs consuming more and more memory over time.

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int *dynamicArray = (int *)malloc(5 * sizeof(int));
    if (dynamicArray == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    // Use dynamicArray
    for (int i = 0; i < 5; i++) {
        dynamicArray[i] = i * 10;
    }

    // Memory leak - forgetting to free allocated memory
    return 0;
}
```

**Stop 3: Ownership and Dangling Pointers**

Another issue in C is the possibility of dangling pointers, which occur when you access memory that has already been deallocated.

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int *dynamicArray = (int *)malloc(5 * sizeof(int));
    if (dynamicArray == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    // Use dynamicArray
    for (int i = 0; i < 5; i++) {
        dynamicArray[i] = i * 10;
    }

    // Deallocate memory
    free(dynamicArray);

    // Accessing memory through a dangling pointer
    printf("%d\n", dynamicArray[0]); // Undefined behavior
    return 0;
}
```

**Stop 4: Ownership and Best Practices**

To manage ownership effectively in C, follow these best practices:

- Always check if memory allocation (`malloc`, `calloc`, etc.) succeeds.
- Free memory using `free` when it's no longer needed.
- Set pointers to `NULL` after freeing memory to avoid dangling pointers.
- Use data structures like linked lists and arrays cautiously, ensuring proper allocation and deallocation.

In C, ownership comes with great power and responsibility. Proper memory management is essential to avoid memory leaks and undefined behavior.
# Cake ownership checks tour


## Introduction

Cake can be used as static analysis tool, generating messages on terminal and also Sarif files.
One distinguish feature is the ownership checks, that is the subject of this tour.

## Owner qualifier
Cake introduced the _\_Owner_ qualifier. This qualifier can be used in similar places where you use const. What this qualifier means is the the type is responsible for some resource, like memory for instance, and cannot be discarded, it must be moved or destroyed before the end of the it's lifetime.

Wait, is is static analysis or language feature? Both. Because there is no change in the generated code, this qualifier can be used as a macro and just be empty for other compilers. For instance, having a `ownership.h` header.

```c  
#ifdef __CAKE__  
#define owner _Onwer
#else
#define owner
#endif 
```

Sample:

```c  
#include "ownership.h"    
#include <stdio.h>
int main() {
  char * owner s = malloc("hello world!");
  printf("%s", s);
}
```

Let's explain the sample. The variable s is now "owner pointer to char". Cake give us two warnings.
1 - Cannot initialize s with a non owner type.
 
Each error comes from different analysis. The first warning introduces our first change in the type system. We cannot initialize or assign a owner type from a non owner. To fix this error we can redeclare malloc as:

```c  
void * owner malloc(size_t sz);  
```
  
2 - object s was not move/destroyed before the end of it's lifetime.
The second warning comes from flow analysis. From my experience, having just ported the cake source to use the owner qualifier helps me to clarify some pointer, wherever they where owner or not. However, the flow analyses will check if the rules, like owner types must be moved or destroyed are being followed.

To fix the second warning we need to move the pointer to the modified free function.  

```c  
void free(void * owner p);  
```

(All these modified declarations can be inside the `ownership.h` header)

Our sample becomes.

```c  
#include "ownership.h"    
#include <stdio.h>
int main() {
  char * owner s = malloc("hello world!");
  printf("%s", s);
  free(s);
}
```

And we have a extra warning. "'s' cannot be implicitly moved to parameter 1 of free". This is the another change in the type system. 

We have just one owner. Assign one owner type the another, or calling a function with owner parameter is the same of transferring ownership.

I make a decision of making this move more explicit requiring a 'move' annotation. To remove the warning we can use `free(move s)`.   

This annotation was already present in my C code in form of comments. For instance,when adding a node in a linked list I use to comment like this.  

```c
list_add(&list, p_item /*MOVED*/)
```

However, when I am calling free for instance I don't put any comments, because it has an obvious semantics.

This style was met adding a attribute [[implicit]] that can be used like:

```c  
void free([[implicit]]void * owner p);  
```

The we can white just `free(s)`.

We have just one owner. Assigment passing this object as  
We have just one owner object, assignment owner = owner, or calling a function with a owner argument type to a owner parameter type is the same of moving ownership.















