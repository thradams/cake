//en.cppreference.com/w/c/io/fwscanf.html
#include <stdio.h>
#include <wchar.h>
#include <string.h>
 
#define NUM_VARS   3
#define ERR_READ   2
#define ERR_WRITE  3
 
int main(void) {
    wchar_t state[64];
    wchar_t capital[64];
    unsigned int population = 0;
    int elevation = 0;
    int age = 0;
    float pi = 0;
 
#if INTERACTIVE_MODE
    wprintf(L"Enter state, age, and pi value: ");
    if (wscanf(L"%ls%d%f", state, &age, &pi) != NUM_VARS) {
        fprintf(stderr, "Error reading input.\n");
        return ERR_READ;
    }
#else
    wchar_t* input = L"California 170 3.141592";
    if (swscanf(input, L"%ls%d%f", state, &age, &pi) != NUM_VARS) {
        fprintf(stderr, "Error reading input.\n");
        return ERR_READ;
    }
#endif
    wprintf(L"State: %ls\nAge  : %d years\nPi   : %.5f\n\n", state, age, pi);
 
    FILE* fp = tmpfile();
    if (fp) {
        // write some data to temp file
        if (!fwprintf(fp, L"Mississippi Jackson 420000 807")) {
            fprintf(stderr, "Error writing to file.\n");
            fclose(fp);
            return ERR_WRITE;
        }
        // rewind file pointer
        rewind(fp);
 
        // read data into variables
        fwscanf(fp, L"%ls%ls%u%d", state, capital, &population, &elevation);
        wprintf(L"State  : %ls\nCapital: %ls\nJackson population (in 2020): %u\n"
                L"Highest elevation: %dft\n",
                state, capital, population, elevation);
        fclose(fp);
    }
}