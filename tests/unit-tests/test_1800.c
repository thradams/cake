typedef struct X X;
struct X
{
    void *data;
};
static_assert(sizeof(X) == sizeof(void *));