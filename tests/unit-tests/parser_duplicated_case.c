int main()
{
    int i = 0;
    switch (i)
    {
        case 1:break;
        case 2: break;
        
        //error: duplicate case '1'

        case 1: 
         break;
    } //lint 1450
}

