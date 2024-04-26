#pragma safety enable



int f();
int main()
{
    int i;
    if (1) /*try*/
    {
        if (f()){
            i = 1;
            goto _catch_label_1;
        }
        i = 0;
    }
    else _catch_label_1:
    {
        
    }
    
}