int main()
{
    int i;
    switch (i)
    {
        case 1:break;
        case 2: break;
        
        //error: duplicate case '1'
        [[cake::e1450]]
        case 1:break;
    }
}

