struct item {
    struct item* next;
};

struct item_list {
    struct item* head;
    struct item* tail;
};

int main() 
{
    struct item_list list = {};
    for (auto p = list.head; p; p = p->next) {
    }
}
