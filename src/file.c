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

void books_destroy(_Implicit struct books* _Obj_owner books)
{
    struct book* _Owner it = books->head;
    while (it != NULL) {
        struct book* _Owner next = _Move it->next;
        book_destroy(it);
        free(it);
        it = _Move next;
    }
}

int main(int argc, char* argv[])
{
    struct books list = { 0 };
    struct book* _Owner b1 = calloc(1, sizeof(struct book));
    if (b1)
    {
        books_push_back(&list, _Move b1);
    }
    books_destroy(&list);
}
