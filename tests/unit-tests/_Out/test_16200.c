#pragma safety enable



struct X {
    char* p;
};


int condition;

void f1()
{
    int i;
    if (1) /*try*/
    {
        if (condition) {
            i = 1;
            goto _catch_label_1;
        }
        if (condition) {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        return;
    }

    
}

void f2()
{
    int i;
    if (1) /*try*/
    {
        if (condition) {
            i = 1;
            goto _catch_label_1;
        }
        if (condition) {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {        
    }

    
}

void f2()
{
    int i;
    if (1) /*try*/
    {
        if (condition) {
            i = 1;
            goto _catch_label_1;
        }
        if (condition) {
            goto _catch_label_1;
        }
        i = 0;
    }
    else _catch_label_1:
    {        
    }

    
}

void f3()
{
    int i;
    if (1) /*try*/
    {
        if (condition) {
            i = 1;
            goto _catch_label_1;
        }
        if (condition) {
            goto _catch_label_1;
        }
        i = 0;
        return;
    }
    else _catch_label_1:
    {        
    }

    
}

void f4()
{
    int i;
    if (1) /*try*/
    {
        if (condition) {            
            goto _catch_label_1;
        }
        if (condition) {
            goto _catch_label_1;
        }       
    }
    else _catch_label_1:
    {        
        i = 0;
    }

    
}


int main()
{

}
