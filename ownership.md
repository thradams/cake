# Ownership rules tour


## Moving function arguments

```c
void list_add(struct list* list, [[cake::move]] struct node* node) 
{
}

int main()
{
   struct list list = {};
   struct node * node = malloc(sizeof (struct node));
   list_add(&list, node);
}
```

## Moving locals

```c

int main()
{
   struct node * node = malloc(sizeof (struct node));
   struct node * p;
   p = [[cake:::move]] node; //explicitly moved
   free(p);
}


```

## Returning locals

```c

struct node* make_node()
{
   struct node * node = malloc(sizeof (struct node));
   return node; /*implicitly moved*/
}

```
