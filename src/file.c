void *owner malloc(int i);
void free(void  *owner);

struct node {
 struct node* owner next;
};

struct list {
  struct node * owner head;
  struct node * tail;
};


void list_destroy(struct list* obj_owner list)
{
  struct node * owner p = list->head;
  while (p) {
      struct node *  owner next = p->next;
      free(p);
      p = next;
  }
  //static_debug(p);
}
