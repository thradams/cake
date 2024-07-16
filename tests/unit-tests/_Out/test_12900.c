#pragma safety enable


void  free(void* /*_Owner*/ p);
char* /*_Owner*/ strdup(const char* s);

struct X {
    char* /*_Owner*/ s;
};
void init(/*_Out*/ struct X* px)
{
    
    
    px->s = strdup("a");
}

int main() {
    struct X x;
    init(&x);
    free(x.s);
}
