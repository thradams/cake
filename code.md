# Code

## dynamic array
### dynamic array of int

```c

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

struct int_array {
    int* data;
    int size;
    int capacity;
};

int int_array_reserve(struct int_array* p, int n)
{
    if (n > p->capacity) {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0])))) {        
            return EOVERFLOW;
        }

        void* pnew = realloc(p->data, n * sizeof(p->data[0]));
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
             new_capacity =  p->capacity + p->capacity / 2;
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

void int_array_destroy(struct int_array* p)
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
```

### dynamic array of struct book

```c


#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

struct book {
    char title[10];
};


struct books {
    struct book* data;
    int size;
    int capacity;
};

int books_reserve(struct books* p, int n)
{
    if (n > p->capacity) {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0])))) {        
            return EOVERFLOW;
        }

        void* pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int books_push_back(struct books* p, const struct book* book)
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
             new_capacity =  p->capacity + p->capacity / 2;
             if (new_capacity < p->size + 1) {
                new_capacity = p->size + 1;
             }
         }

        int error = books_reserve(p, new_capacity);
        if (error != 0) {
            return error;
        }
     }

     p->data[p->size] = *book; /*COPIED*/


    p->size++;

    return 0;
}

void books_destroy(struct books* books)
{

    free(books->data);
}

int main()
{
   struct books books = { 0 };

   struct book book = { .title ="book1" };
   books_push_back(&books, &book/*COPIED*/);
   books_destroy(&books);
}

```

### dynamic array of pointer to struct book

```c



#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

struct book {
    char title[10];
};



struct books {
    struct book** data;
    int size;
    int capacity;
};

int books_reserve(struct books* p, int n)
{
    if (n > p->capacity) {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0])))) {        
            return EOVERFLOW;
        }

        void* pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int books_push_back(struct books* p, struct book* book)
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
             new_capacity =  p->capacity + p->capacity / 2;
             if (new_capacity < p->size + 1) {
                new_capacity = p->size + 1;
             }
         }

        int error = books_reserve(p, new_capacity);
        if (error != 0) {
            return error;
        }
     }



    p->data[p->size] = book; /*VIEW*/
    p->size++;

    return 0;
}

void books_destroy(struct books* books)
{

    free(books->data);
}

int main()
{
   struct books books = { 0 };

   struct book book = { .title = "book1" };
   books_push_back(&books, &book/*VIEW*/);
   books_destroy(&books);
}

```

### dynamic array of owner pointer to struct book

```c
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#ifdef _MSC_VER
#define strdup _strdup
#endif

struct book {
    char* title;
};


void item_delete(struct book* book)
{
    if (book) {
       free(book->title);
    }
}

struct books {
    struct book** data;
    int size;
    int capacity;
};
int books_reserve(struct books* p, int n)
{
    if (n > p->capacity) {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0])))) {        
            return EOVERFLOW;
        }

        void* pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int books_push_back(struct books* p, struct book* book)
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
             new_capacity =  p->capacity + p->capacity / 2;
             if (new_capacity < p->size + 1) {
                new_capacity = p->size + 1;
             }
         }

        int error = books_reserve(p, new_capacity);
        if (error != 0) {
            return error;
        }
     }



    p->data[p->size] = book; /*MOVED*/
    p->size++;

    return 0;
}

void books_destroy(struct books* books)
{

    for (int i = 0; i < books->size; i++) {
        item_delete(books->data[i]);
    }
    free(books->data);
}

int main()
{
   struct books books = { 0 };

   struct book* book = calloc(1, sizeof (struct book));
   if (book == NULL) goto continuation;

   book->title = strdup("book1");
   if (book->title == NULL) goto continuation;
   
   if (books_push_back(&books, book) == 0) {
     book = NULL; /*MOVED*/
   }

  continuation:

  item_delete(book);
  books_destroy(&books);
}
```

### dynamic array of struct book (move)

```c
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#ifdef _MSC_VER
#define strdup _strdup
#endif

struct book {
    char* title;
};

void item_destroy(struct book* book)
{
    free(book->title);
}


struct books {
    struct book* data;
    int size;
    int capacity;
};
int books_reserve(struct books* p, int n)
{
    if (n > p->capacity) {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0])))) {        
            return EOVERFLOW;
        }

        void* pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int books_push_back(struct books* p, struct book* book)
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
             new_capacity =  p->capacity + p->capacity / 2;
             if (new_capacity < p->size + 1) {
                new_capacity = p->size + 1;
             }
         }

        int error = books_reserve(p, new_capacity);
        if (error != 0) {
            return error;
        }
     }

     p->data[p->size] = *book; /*MOVED*/

    memset(book, 0, sizeof(*book));

    p->size++;

    return 0;
}

void books_destroy(struct books* books)
{
    for (int i = 0; i < books->size; i++) {
        item_destroy(&books->data[i]);
    }

    free(books->data);
}

int main()
{
   struct books books = { 0 };

   struct book book = { 0 };
   book.title = strdup("title 1");
   if (book.title == NULL) goto continuation;
    
   if (books_push_back(&books, &book) == 0) {
      /*MOVED ON SUCCESS*/
   }
    
   continuation:

   item_destroy(&book);
   books_destroy(&books);
}
```
## Linked list

### Single linked list

```c
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

struct item {
     char* title;
     struct item* next;
};

void item_destroy(struct item* item) {
     free(item->title);
}
 

struct list {
    struct item* head, *tail;
};

void list_insert_after(struct list* list, struct item* item, struct item* new_book)
{
    assert(list != NULL);
    assert(item != NULL);
    assert(new_book != NULL);
    assert(new_book->next == NULL);


    if (item->next == NULL) {
        list->tail = new_book;
    }
    else {
        new_book->next = item->next;
    }

    item->next = new_book;
}


void list_push_back(struct list* list, struct item* new_book)
{
   assert(list != NULL);
   assert(new_book != NULL);
   assert(new_book->next == NULL);

   if (list->tail == NULL) {
      list->head = new_book;
   }
   else {
      list->tail->next = new_book;
   }
   list->tail = new_book;
}

void list_push_front(struct list* list, struct item* new_book)
{
    assert(list != NULL);
    assert(new_book != NULL);
    assert(new_book->next == NULL);

    if (list->head == NULL) {
        list->tail = new_book;
    }
    else {
        new_book->next = list->head;        
    }
    list->head = new_book;
}

void list_destroy(struct list* list)
{
    //pre condition
    assert(list != NULL);

    struct item* it = list->head;
    while (it != NULL) {
        struct item* next = it->next;
        item_destroy(it);
        free(it);
        it = next;
    }
}

int main(int argc, char* argv[])
{
    struct list list = { 0 };
    struct item* b1 = calloc(1, sizeof(struct item));
    if (b1)
    {
        list_push_front(&list, b1);
    }
    list_destroy(&list);
}
```
### double linked list

```c
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

struct item {
     char* title;
     struct item* next;
     struct item* prev;
};

void item_destroy(struct item* item) {
     free(item->title);
}
 
struct list {
    struct item* head, *tail;
};

void list_insert_after(struct list* list, struct item* item, struct item* new_book)
{
    assert(list != NULL);
    assert(item != NULL);
    assert(new_book != NULL);
    assert(new_book->prev == NULL);
    assert(new_book->next == NULL);

    new_book->prev = item;

    if (item->next == NULL) {
        list->tail = new_book;
    }
    else {
        new_book->next = item->next;
        item->next->prev = new_book;
    }

    item->next = new_book;
}

void list_insert_before(struct list* list, struct item* item, struct item* new_book)
{
    assert(list != NULL);
    assert(item != NULL);
    assert(new_book != NULL);
    assert(new_book->prev == NULL);
    assert(new_book->next == NULL);

    new_book->next = item;
    if (item->prev == NULL) {
        list->head = new_book;
    }
    else {
        new_book->prev = item->prev;
        item->prev->next = new_book;
    }
    item->prev = new_book;

}
void list_remove(struct list* list, struct item* item)
{
    assert(list != NULL);
    assert(item != NULL);

    if (item->prev == NULL)
        list->head = item->next;
    else
        item->prev->next = item->next;

    if (item->next == NULL)
        list->tail = item->prev;
    else
        item->next->prev = item->prev;
    
    item_destroy(item);
    free(item);
}


void list_push_back(struct list* list, struct item* new_book)
{
   assert(list != NULL);
   assert(new_book != NULL);
   assert(new_book->prev == NULL);
   assert(new_book->next == NULL);

   if (list->tail == NULL) {
      list->head = new_book;
   }
   else {
      new_book->prev = list->tail;        
      list->tail->next = new_book;
   }
   list->tail = new_book;
}

void list_push_front(struct list* list, struct item* new_book)
{
    assert(list != NULL);
    assert(new_book != NULL);
     assert(new_book->prev == NULL);
    assert(new_book->next == NULL);

    if (list->head == NULL) {
        list->tail = new_book;
    }
    else {
        new_book->next = list->head;        
        list->head->prev = new_book;
    }
    list->head = new_book;
}

void list_destroy(struct list* list)
{
    //pre condition
    assert(list != NULL);

    struct item* it = list->head;
    while (it != NULL) {
        struct item* next = it->next;
        item_destroy(it);
        free(it);
        it = next;
    }
}

int main(int argc, char* argv[])
{
    struct list list = { 0 };
    struct item* b1 = calloc(1, sizeof(struct item));
    if (b1)
    {
        list_push_front(&list, b1);
    }
    list_destroy(&list);
}
```
