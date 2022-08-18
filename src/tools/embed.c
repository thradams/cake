#include <stdio.h>

int embed(const char* filename)
{
    char file_out_name[200] = { 0 };
    if (snprintf(file_out_name, sizeof file_out_name, "%s.include", filename) >= sizeof file_out_name)
        return 0;

    FILE* file_out = fopen(file_out_name, "w");
    if (file_out == NULL)
        return 0;

    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fclose(file_out);
        return 0;
    }

    int count = 0;
    unsigned char ch;
    while (fread(&ch, 1, 1, file)) {
        if (count % 25 == 0)
            fprintf(file_out, "\n");

        if (count > 0)
            fprintf(file_out, ",");

        fprintf(file_out, "%d", (int)ch);
        count++;
    }

    fclose(file);
    fclose(file_out);
    return count;
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        int bytes = embed(argv[i]);
        if (bytes == 0) {
            printf("error generating file %s\n", argv[i]);
        }
    }
}
