#pragma safety enable


void destroy(/*_Owner*/ int i);

int main()
{
    /*_Owner*/ int i = 0;
    int v = i;
    destroy(v);
    #pragma cake diagnostic check "-Wnon-owner-to-owner-move"
}
#pragma cake diagnostic check "-Wmissing-destructor"


