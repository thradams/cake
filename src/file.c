
void* owner malloc(int i);

struct X {
    void* owner text;
};

void x_change(struct X* list);

int main(int condition)
{
    struct X* owner x = malloc(sizeof * x);
    if (condition)
    {
        x_change(x);
    }
}
