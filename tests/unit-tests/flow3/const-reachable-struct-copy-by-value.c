/* a struct copied by value carries its pointers without the const of where it came from */

struct item { int value; };
struct holder { struct item* item; };
struct plain { int a; int b; };

void take(struct holder h);

void copy(const struct holder* p)
{
    struct holder h = *p; //lint 120
    h.item->value = 1;
}

struct holder get(const struct holder* p)
{
    return *p; //lint 120
}

void pass(const struct holder* p)
{
    take(*p); //lint 120
}

struct plain no_pointers(const struct plain* p)
{
    return *p;
}