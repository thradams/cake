void* /*_Owner*/ malloc(int i);
void free(void* /*_Owner*/ _Opt p);
int rand();

int main()
{
    char* /*_Owner*/ s = malloc(1);
    if (1) /*try*/
    {
        if (rand())
        {
            free(s);
        }
        else
        {
            
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
    }    
}
#pragma cake diagnostic check "-Wmissing-destructor"
