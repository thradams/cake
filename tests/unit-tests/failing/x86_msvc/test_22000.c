/* Cake x86_msvc */

int *fopen(char * filename, char * mode);
int fclose(int * stream);

int main()
{
    {
        int * f = fopen("file.txt", "r");
        if (f)
        {
            fclose(f);
        }
    }
}


