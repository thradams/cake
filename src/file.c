enum static_analisys_flags
{
    ISVALID = 1 << 1,
    UNINITIALIZED = 1 << 2,
    MUST_DESTROY = 1 << 3,
    MUST_FREE = 1 << 4
};


void free(void* ptr) extern {
    _del_attr(ptr, MUST_FREE);
}

void free(void* ptr);

void free(void* ptr)
{
}
int main()
{
    int* p;
    _add_attr(p, MUST_FREE);
    free(p);
}