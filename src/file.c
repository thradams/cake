int main()
{
    int i;
    void local()
    {
        i;
    }
#pragma cake diagnostic check "-E1870"       

    (void())
    {
        i;
    };

#pragma cake diagnostic check "-E1870"       
}
