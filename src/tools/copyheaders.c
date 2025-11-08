/*
   This program copies files from a list do a dest dir
   cake all_headers.h -H
   copy the output for a file.
   then run
   copyheaders files.txt include

   generates something like

   c:\\program files (x86)\\windows kits\\10\\include\\10.0.26100.0\\ucrt\\assert.h
    ..c:\\program files (x86)\\windows kits\\10\\include\\10.0.26100.0\\ucrt\\corecrt.h
    ...c:\\program files\\microsoft visual studio\\2022\\community\\vc\\tools\\msvc\\14.44.35207\\include\\vcruntime.h
    ....c:\\program files\\microsoft visual studio\\2022\\community\\vc\\tools\\msvc\\14.44.35207\\include\\sal.h
    .....c:\\program files\\microsoft visual studio\\2022\\community\\vc\\tools\\msvc\\14.44.35207\\include\\concurrencysal.h
    ....c:\\program files\\microsoft visual studio\\2022\\community\\vc\\tools\\msvc\\14.44.35207\\include\\vadefs.h
    .c:\\program files (x86)\\windows kits\\10\\include\\10.0.26100.0\\ucrt\\errno.h
    .c:\\program files\\microsoft visual studio\\2022\\community\\vc\\tools\\msvc\\14.44.35207\\include\\limits.h

*/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define PATH_SEP '\\\\'
#else
#include <unistd.h>
#include <sys/stat.h>
#define PATH_SEP '/'
#endif

#define MAX_PATH_LEN 4096
#define BUFFER_SIZE 8192

// Function to copy a single file
int copy_file(const char* src_path, const char* dst_path)
{
    FILE* src = fopen(src_path, "rb");
    if (!src)
    {
        perror(src_path);
        return -1;
    }

    FILE* dst = fopen(dst_path, "wb");
    if (!dst)
    {
        perror(dst_path);
        fclose(src);
        return -1;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0)
    {
        if (fwrite(buffer, 1, bytes, dst) != bytes)
        {
            perror("fwrite");
            fclose(src);
            fclose(dst);
            return -1;
        }
    }

    fclose(src);
    fclose(dst);
    return 0;
}

// Get filename from a full path
const char* get_filename(const char* path)
{

    const char* p = strchr(path, PATH_SEP);
    p = strchr(p + 1, PATH_SEP);

    return p ? p + 1 : path;
}



static int create_multiple_paths(const char* root)
{
    /*
       This function creates all dirs (folder1, forder2 ..) after root
       root   : C:/folder
       outdir : C:/folder/folder1/folder2 ...
    */
    char outdir[255];
    const char* p = root;
    int count = 0;
    for (;;)
    {
        if (*p != '\\0' && *p != '/' && *p != '\\\\')
        {
            count++;
            p++;
            continue;
        }

        if (*p == '\\0')
            break;

        char temp[250] = { 0 };
        strncpy(temp, root, count);

        int er = mkdir(temp/*, 0777*/);
        if (er != 0)
        {
            er = errno;
            if (er != EEXIST)
            {
                return er;
            }
        }
        if (*p == '\\0')
            break;
        p++;
        count++;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf( "\n");
        printf( " Steps:\n");
        printf( " 1) cake all_headers.h -H\n");
        printf( "    This command lists all headers used. Execute it with both GCC and MSVC.\n");
        printf( "    (all_headers is a header file that includes the headers you need)\n");
        printf( "\n");
        printf( " It will print something like this:\n");
        printf( "    C:/Users/thiag/source/repos/cake_private/src/all_headers.h\n");
        printf( "    .C:/Users/thiag/source/repos/cake_private/src/cakeconfig.h\n");
        printf( "    .c:\\users\\thiag\\source\\repos\\cake_private\\src\\include\\usr\\include\\assert.h\n");
        printf( "    ..c:\\users\\thiag\\source\\repos\\cake_private\\src\\include\\usr\\include\\features.h\n");
        printf( "    ...c:\\users\\thiag\\source\\repos\\cake_private\\src\\include\\usr\\include\\features-time64.h\n");
        printf( "    ...more...\n");
        printf( "\n");

        printf( " 2) Save the output to a file named files.txt, then remove lines with all_headers.h and cakeconfig.h\n");
        printf( "\n");
        printf( " 3) Run: copyheaders files.txt include_light\n");
        printf( "    This command copies all the listed header files to the include_light directory.\n");
        printf( "\n");

        return 1;
    }

    const char* list_path = argv[1];
    const char* dst_dir = argv[2];

    FILE* list = fopen(list_path, "r");
    if (!list)
    {
        perror(list_path);
        return 1;
    }

    char line[MAX_PATH_LEN];
    while (fgets(line, sizeof(line), list))
    {
        // Remove newline
        size_t len = strlen(line);
        if (len > 0 && (line[len - 1] == '\\n' || line[len - 1] == '\\r'))
            line[len - 1] = '\\0';

        if (line[0] == '\\0') continue; // skip empty lines

        const char* start = line;
        while (*start == '.')
            start++;

        // Build destination path
        char dst_path[MAX_PATH_LEN];
        snprintf(dst_path, sizeof(dst_path), "%s%c%s", dst_dir, PATH_SEP, get_filename(start));

        printf("Copying: %s -> %s\\n", start, dst_path);


        create_multiple_paths(dst_path);

        if (copy_file(start, dst_path) != 0)
        {
            fprintf(stderr, "Failed to copy: %s\\n", start);
        }
    }

    fclose(list);
    return 0;
}
