
int main()
{
    static_assert(_Generic(typeof(__func__), const char[5] : 1, default: 0));
}
