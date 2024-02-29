extern void puts(const char*);


typedef struct {} x86_64;
typedef struct {} i386;

struct x86_64 { x86_64 type; };
struct i386 { i386   type; };

typedef union {
    typeof(
        _Generic(
            &(char[sizeof sizeof 1]) { 0 },
            char(*)[4]:(struct i386) {}.type,
            char(*)[8] : (struct x86_64) {}.type)
    ) cpu_type;
} arch;

int main(void)
{
    puts(_Generic(
        (arch*)
    {
        0
    }->cpu_type,
        x86_64:"x86_64",
            i386 : "i386"));
}