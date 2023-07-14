void * _Owner calloc(int n, int sz);
void free(_Implicit void * _Owner);
#define NULL ((void*) 0)

struct book {
     char* _Owner title;
     struct book* _Owner next;
};

void book_destroy(_Implicit struct book* _Obj_owner book) {
     free(book->title);
}
 

struct books {
    struct book* _Owner head, *tail;
};


void books_push_back(struct books* books, struct book* _Owner new_book)
{
   if (books->tail == NULL) {
      books->head = _Move new_book;
   }
   else {
      books->tail->next = _Move new_book;
   }
   books->tail = new_book;
}