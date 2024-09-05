#pragma safety enable

void free(void* _Owner _Opt ptr);

struct item {
     char* _Owner title;
     struct item* _Owner _Opt  next;
};

struct list {
    struct item* _Owner _Opt head;
    struct item* _Opt tail;
};


void list_destroy(struct list* _Obj_owner list)
{
    struct item* _Owner _Opt it = list->head;
    
    while (it != nullptr) {
        struct item* _Owner _Opt next = it->next;
        free(it->title);
        free(it);
        it = next;        
    }
    static_debug(it);
}
