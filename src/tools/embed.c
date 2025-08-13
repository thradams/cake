#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <fs.h>
#include <stdlib.h>
#include <string.h>
 

int embed(const char* filename)

{

    char file_out_name[200] = { 0 };

    if (snprintf(file_out_name, sizeof file_out_name, "%s.include", filename) >= sizeof file_out_name)

        return 0;



    FILE* file_out = fopen(file_out_name, "w");

    if (file_out == NULL)

        return 0;



    FILE* file = fopen(filename, "rb");

    if (file == NULL)
    {

        fclose(file_out);

        return 0;

    }



    int count = 0;

    unsigned char ch;

    while (fread(&ch, 1, 1, file))
    {

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



int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("usage: embed dirname");
        return 1;
    }
    char* path = argv[1];
    DIR* dir = opendir(path);

    if (dir == NULL)
    {
        return errno;
    }

    struct dirent* dp;
    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            /* skip self and parent */
            continue;
        }

        if (dp->d_type & DT_DIR)
        {
        }
        else
        {
            char filepath[257] = { 0 };
            snprintf(filepath, sizeof filepath, "%s/%s", path, dp->d_name);
            const char* const file_extension = strrchr((char*)filepath, '.');

            if (strcmp(file_extension, ".include") == 0)
            {
                continue;
            }

            int bytes = embed(filepath);

            if (bytes == 0)
            {
                printf("error generating file %s\n", filepath);
                exit(1);
            }
            else
            {
              printf("embed generated '%s'\n", filepath);
            }
        }
    }
    closedir(dir);
}

