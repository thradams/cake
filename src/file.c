
typedef unsigned char u8;

int main()
{
    const u8 p;
    static_assert((typeof(p)) == (const unsigned char));
}