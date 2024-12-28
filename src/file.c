#pragma safety enable

struct item {
     char* _Owner title;
     struct item* _Owner _Opt  next;
};

struct list {
    struct item* _Owner _Opt head;
    struct item* _Opt tail;
};

bool list_is_empty(const struct list* list2)
  true(list2.head == nullptr && list2.tail == nullptr),
  false(list2.head != nullptr && list2.tail != nullptr)
{
    return list2->head != nullptr;
}

void list_clear(struct list* list1)
  pos(list_is_empty(list1))
{
    list1->head = 0;
}


int main(int argc, char* argv[])
{
  struct list l = {0};
  list_clear(&l);
  static_state(l.head , "null");
}
