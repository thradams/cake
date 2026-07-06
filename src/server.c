// server.c  –  cross-platform (Windows + Linux/macOS)
//              all allocations are sized to actual data – no hard-coded limits

/* ── Windows preamble ────────────────────────────────────────────────── */
#ifdef _WIN32

#ifndef _CRT_SECURE_NO_WARNINGS
#  define _CRT_SECURE_NO_WARNINGS
#endif

#  include <winsock2.h>
#  include <windows.h>
#  pragma comment(lib, "Ws2_32.lib")
#  pragma comment(lib, "Shell32.lib")
typedef SOCKET SocketFd;
#  define CLOSE_SOCKET(s)  closesocket(s)
#  define PATH_SEP         "\\"
#  define strcasecmp       _stricmp
#  define strncasecmp      _strnicmp
#  define strdup           _strdup

/* ── POSIX preamble ──────────────────────────────────────────────────── */
#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <strings.h>
typedef int SocketFd;
#  define CLOSE_SOCKET(s)  close(s)
#  define INVALID_SOCKET   (-1)
#  define SOCKET_ERROR     (-1)
#  define PATH_SEP         "/"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ─────────────────────────────────────────────────────────────────────── */
#define PORT         8080
#define MAX_PATH_LEN 2048   /* max filesystem path length */
#define MAX_CMD_LEN  (MAX_PATH_LEN * 2 + 64)


char BASE_DIR[512];
char BASE_DIR2[512];

/* ══════════════════════════════════════════════════════════════════════
   dynbuf  –  a simple growable byte buffer
   ══════════════════════════════════════════════════════════════════════ */
typedef struct {
    char* data;
    size_t len;   /* bytes written (excluding NUL terminator) */
    size_t cap;   /* total bytes allocated                    */
} DynBuf;

static int sv_db_init(DynBuf* b, size_t initial)
{
    b->data = malloc(initial);
    if (!b->data) return -1;
    b->data[0] = '\0';
    b->len = 0;
    b->cap = initial;
    return 0;
}

static int sv_db_ensure(DynBuf* b, size_t extra)
{
    if (b->len + extra + 1 <= b->cap) return 0;
    size_t newcap = b->cap * 2;
    while (newcap < b->len + extra + 1) newcap *= 2;
    char* p = realloc(b->data, newcap);
    if (!p) return -1;
    b->data = p;
    b->cap = newcap;
    return 0;
}

static int sv_db_append(DynBuf* b, const char* s, size_t n)
{
    if (sv_db_ensure(b, n) != 0) return -1;
    memcpy(b->data + b->len, s, n);
    b->len += n;
    b->data[b->len] = '\0';
    return 0;
}

static int sv_db_appends(DynBuf* b, const char* s)
{
    return sv_db_append(b, s, strlen(s));
}

static void sv_db_free(DynBuf* b)
{
    free(b->data);
    b->data = NULL;
    b->len = b->cap = 0;
}

/* ══════════════════════════════════════════════════════════════════════
   sv_file_read_all  –  read an entire file into a malloc'd, NUL-terminated
                     buffer; *out_len receives the byte count.
   ══════════════════════════════════════════════════════════════════════ */
static char* sv_file_read_all(const char* path, size_t* out_len)
{
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;

    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return NULL; }
    rewind(f);

    char* buf = malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return NULL; }

    size_t got = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    buf[got] = '\0';
    if (out_len) *out_len = got;
    return buf;
}

/* ══════════════════════════════════════════════════════════════════════
   path / MIME helpers
   ══════════════════════════════════════════════════════════════════════ */
static const char* sv_get_content_type(const char* path)
{
    const char* ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";
    if (strcasecmp(ext, ".html") == 0) return "text/html";
    if (strcasecmp(ext, ".js") == 0) return "application/javascript";
    if (strcasecmp(ext, ".css") == 0) return "text/css";
    if (strcasecmp(ext, ".txt") == 0) return "text/plain";
    if (strcasecmp(ext, ".json") == 0) return "application/json";
    return "application/octet-stream";
}

static int sv_has_c_extension(const char* name)
{
    const char* dot = strrchr(name, '.');
    if (!dot) return 0;

    //if (strcasecmp(dot, ".h") == 0)
    //    return 1;

    return strcasecmp(dot, ".c") == 0;
}

static int sv_is_safe(const char* s)
{
    return strstr(s, "..") == NULL;
}

static void sv_get_param(const char* query, const char* key,
    char* out, int max)
{
    const char* p = strstr(query, key);
    if (!p) return;
    p += strlen(key);
    int i = 0;
    while (*p && *p != '&' && i < max - 1)
        out[i++] = *p++;
    out[i] = '\0';
}

/* ══════════════════════════════════════════════════════════════════════
   sv_send_response  –  header sized exactly to content length
   ══════════════════════════════════════════════════════════════════════ */
static void sv_send_response(SocketFd client,
    const char* type,
    const char* body, size_t body_len)
{
    /* dry-run snprintf to compute exact header size */
    int hlen = snprintf(NULL, 0,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Length: %zu\r\n"
        "\r\n",
        type, body_len);
    if (hlen <= 0) return;

    char* header = malloc((size_t)hlen + 1);
    if (!header) return;

    snprintf(header, (size_t)hlen + 1,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Length: %zu\r\n"
        "\r\n",
        type, body_len);

    send(client, header, hlen, 0);
    send(client, body, (int)body_len, 0);
    free(header);
}

/* convenience for plain C-string bodies */
static void sv_send_str(SocketFd client, const char* type, const char* body)
{
    sv_send_response(client, type, body, strlen(body));
}

/* ══════════════════════════════════════════════════════════════════════
   sv_handle_static
   ══════════════════════════════════════════════════════════════════════ */
static void sv_handle_static(SocketFd client, const char* url)
{
    char path[MAX_PATH_LEN];

    if (strcmp(url, "/") == 0)
        url = PATH_SEP "index.html";

    snprintf(path, sizeof(path), "%s%s", BASE_DIR, url);

    if (strstr(path, ".."))
    {
        sv_send_str(client, "text/plain", "Invalid path");
        return;
    }

    size_t flen;
    char* buf = sv_file_read_all(path, &flen);
    if (!buf)
    {
        sv_send_str(client, "text/plain", "Not found");
        return;
    }

    sv_send_response(client, sv_get_content_type(path), buf, flen);
    free(buf);
}

/* ══════════════════════════════════════════════════════════════════════
   sv_handle_list  –  JSON array of .c filenames, built with DynBuf
   ══════════════════════════════════════════════════════════════════════ */
static void sv_handle_list(SocketFd client, const char* query)
{
    char rel[512] = "";
    sv_get_param(query, "path=", rel, sizeof(rel));

    if (!sv_is_safe(rel))
    {
        sv_send_str(client, "text/plain", "Invalid path");
        return;
    }

    DynBuf json;
    if (sv_db_init(&json, 256) != 0) return;
    sv_db_appends(&json, "[");
    int first = 1;

#ifdef _WIN32
    char search[MAX_PATH_LEN];
    if (rel[0])
        snprintf(search, sizeof(search), "%s\\%s\\*", BASE_DIR2, rel);
    else
        snprintf(search, sizeof(search), "%s\\*", BASE_DIR2);

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(search, &fd);

    if (h == INVALID_HANDLE_VALUE)
    {
        sv_send_str(client, "text/plain", "Cannot open directory");
        sv_db_free(&json);
        return;
    }
    do
    {
        if (strcmp(fd.cFileName, ".") == 0 ||
            strcmp(fd.cFileName, "..") == 0) continue;
        if (!sv_has_c_extension(fd.cFileName)) continue;
        if (!first) sv_db_appends(&json, ",");
        first = 0;
        sv_db_appends(&json, "\"");
        sv_db_appends(&json, fd.cFileName);
        sv_db_appends(&json, "\"");
    } while (FindNextFileA(h, &fd));
    FindClose(h);

#else
    char dirpath[MAX_PATH_LEN];
    if (rel[0])
        snprintf(dirpath, sizeof(dirpath), "%s/%s", BASE_DIR2, rel);
    else
        snprintf(dirpath, sizeof(dirpath), "%s", BASE_DIR2);

    DIR* dir = opendir(dirpath);

    if (!dir)
    {
        sv_send_str(client, "text/plain", "Cannot open directory");
        sv_db_free(&json);
        return;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) continue;
        if (!sv_has_c_extension(entry->d_name)) continue;
        if (!first) sv_db_appends(&json, ",");
        first = 0;
        sv_db_appends(&json, "\"");
        sv_db_appends(&json, entry->d_name);
        sv_db_appends(&json, "\"");
    }
    closedir(dir);
#endif

    sv_db_appends(&json, "]");
    sv_send_response(client, "application/json", json.data, json.len);
    sv_db_free(&json);
}


static void sv_handle_read(SocketFd client, const char* query)
{
    char rel[512] = "";
    char file[512] = "";
    sv_get_param(query, "path=", rel, sizeof(rel));
    sv_get_param(query, "file=", file, sizeof(file));

    if (!sv_is_safe(rel) || !sv_is_safe(file))
    {
        sv_send_str(client, "text/plain", "Invalid path");
        return;
    }

    char full[MAX_PATH_LEN];
    snprintf(full, sizeof(full), "%s" PATH_SEP "%s" PATH_SEP "%s",
        BASE_DIR2, rel, file);

    size_t flen;
    char* src = sv_file_read_all(full, &flen);
    if (!src)
    {
        sv_send_str(client, "text/plain", "Cannot read file");
        return;
    }

    sv_send_response(client, "text/plain", src, flen);
    free(src);
}


static size_t sv_first_line_copy(const char* src, char* dst, size_t dst_size)
{
    size_t i = 0;

    if (!src)
        return 0;

    /* Copy up to newline or end of string */
    while (src[i] != '\0' && src[i] != '\n')
    {
        if (dst && i + 1 < dst_size)  /* keep space for '\0' */
        {
            dst[i] = src[i];
        }
        i++;
    }

    /* Null-terminate if possible */
    if (dst && dst_size > 0)
    {
        size_t term = (i < dst_size - 1) ? i : (dst_size - 1);
        dst[term] = '\0';
    }

    return i; /* full length of first line (not truncated) */
}

/* ══════════════════════════════════════════════════════════════════════
   sv_handle_save  –  POST {"path":...,"file":...,"content":...}
                   Write file, return "OK"
   ══════════════════════════════════════════════════════════════════════ */
static void sv_handle_save(SocketFd client, const char* body, size_t body_len)
{
    printf("sv_handle_save\n");

    char file[512] = "";
    const char* content; size_t content_len;
    size_t sz = sv_first_line_copy(body, file, sizeof file);
    content = body + sz + 1;
    content_len = strlen(content);

    printf("filename '%s'\n", file);

    char full[MAX_PATH_LEN];
    snprintf(full, sizeof(full), "%s" PATH_SEP "%s",
        BASE_DIR2, file);

    FILE* f = fopen(full, "wb");
    if (!f)
    {
        sv_send_str(client, "text/plain", "Cannot write file");
        return;
    }
    fwrite(content, 1, content_len, f);
    fclose(f);

    sv_send_str(client, "text/plain", "OK");
    (void)body_len;
}


/* ══════════════════════════════════════════════════════════════════════
   sv_handle_compile  –  POST body: "<opts>\n<file>"
                      opts may be empty (just "\n<file>" or "<file>" alone)
                      Compile the already-saved file, return output only
   ══════════════════════════════════════════════════════════════════════ */
static void sv_handle_compile(SocketFd client, const char* body, size_t body_len)
{
    char opts[512] = "";
    char file[512] = "";
    size_t opts_len = sv_first_line_copy(body, opts, sizeof opts);

    printf("sv_handle_compile: opts='%s', file='%s'\n", opts, file);

    /* filename is on the second line */
    const char* file_start = body + opts_len + (body[opts_len] == '\n' ? 1 : 0);
    sv_first_line_copy(file_start, file, sizeof file);

    char full[MAX_PATH_LEN];
    snprintf(full, sizeof(full), "%s" PATH_SEP "%s",
        BASE_DIR2, file);

    char cmd[MAX_CMD_LEN] = { 0 };
    if (opts[0])
        snprintf(cmd, sizeof(cmd), "cake  %s \"%s\" > output.txt", opts, full);
    else
        snprintf(cmd, sizeof(cmd), "cake  \"%s\" > output.txt", full);

    printf("%s\n", cmd);
    system(cmd);

    size_t out_len;
    char* out = sv_file_read_all("output.txt", &out_len);
    if (!out) { out = strdup("(no compiler output)"); out_len = out ? strlen(out) : 0; }
    if (!out) return;
    sv_send_response(client, "text/plain", out, out_len);
    free(out);
    (void)body_len;
}


/* ══════════════════════════════════════════════════════════════════════
   sv_json_escape  –  append src to b with JSON string escaping
   ══════════════════════════════════════════════════════════════════════ */
static int sv_json_escape(DynBuf* b, const char* src, size_t src_len)
{
    for (size_t i = 0; i < src_len; i++)
    {
        unsigned char c = (unsigned char)src[i];
        if (c == '"') { if (sv_db_append(b, "\\\"", 2) != 0) return -1; }
        else if (c == '\\') { if (sv_db_append(b, "\\\\", 2) != 0) return -1; }
        else if (c == '\n') { if (sv_db_append(b, "\\n", 2) != 0) return -1; }
        else if (c == '\r') { if (sv_db_append(b, "\\r", 2) != 0) return -1; }
        else if (c == '\t') { if (sv_db_append(b, "\\t", 2) != 0) return -1; }
        else if (c < 0x20)
        {
            char esc[7];
            snprintf(esc, sizeof(esc), "\\u%04x", c);
            if (sv_db_append(b, esc, 6) != 0) return -1;
        }
        else { if (sv_db_append(b, (const char*)&c, 1) != 0) return -1; }
    }
    return 0;
}

/* ══════════════════════════════════════════════════════════════════════
   sv_handle_build  –  POST body: "<opts>\n<source_code>"
                       Saves source to temp.c, runs cake, returns JSON:
                       {"output":"<diagnostics>","code":"<transformed_c>"}
   ══════════════════════════════════════════════════════════════════════ */
static void sv_handle_build(SocketFd client, const char* body, size_t body_len)
{
    /* ── 1. split opts / source ── */
    char opts[512] = "";
    size_t opts_len = sv_first_line_copy(body, opts, sizeof opts);
    const char* source = body + opts_len + (body[opts_len] == '\n' ? 1 : 0);
    size_t source_len = body_len > (opts_len + 1) ? body_len - opts_len - 1 : 0;

    /* ── 2. write source to temp.c ── */
    char input_path[MAX_PATH_LEN];
    snprintf(input_path, sizeof(input_path), "%s" PATH_SEP "temp.c", BASE_DIR2);

    FILE* f = fopen(input_path, "wb");
    if (!f)
    {
        sv_send_str(client, "application/json",
                  "{\"output\":\"Cannot write temp.c\",\"code\":\"\"}"); return;
    }
    fwrite(source, 1, source_len, f);
    fclose(f);

    /* ── 3. run cake; diagnostics → output.txt, code → out.c ── */
    char out_path[MAX_PATH_LEN];
    snprintf(out_path, sizeof(out_path), "%s" PATH_SEP "out.c", BASE_DIR2);

    char cmd[MAX_CMD_LEN] = { 0 };
    if (opts[0])
        snprintf(cmd, sizeof(cmd), "cake %s -o \"%s\" \"%s\" > output.txt 2>&1", opts, out_path, input_path);
    else
        snprintf(cmd, sizeof(cmd), "cake -o \"%s\" \"%s\" > output.txt 2>&1", out_path, input_path);

    printf("sv_handle_build: %s\n", cmd);
    system(cmd);

    /* ── 4. read diagnostics ── */
    size_t diag_len = 0;
    char* diag = sv_file_read_all("output.txt", &diag_len);
    if (!diag) { diag = strdup(""); diag_len = 0; }

    /* ── 5. read transformed output ── */
    size_t code_len = 0;
    char* code = sv_file_read_all(out_path, &code_len);
    if (!code) { code = strdup(""); code_len = 0; }

    /* ── 6. build JSON response ── */
    DynBuf json;
    if (sv_db_init(&json, diag_len + code_len + 32) != 0)
    {
        free(diag); free(code); return;
    }
    sv_db_appends(&json, "{\"output\":\"");
    sv_json_escape(&json, diag, diag_len);
    sv_db_appends(&json, "\",\"code\":\"");
    sv_json_escape(&json, code, code_len);
    sv_db_appends(&json, "\"}");

    sv_send_response(client, "application/json", json.data, json.len);

    sv_db_free(&json);
    free(diag);
    free(code);
    (void)body_len;
}

/* ══════════════════════════════════════════════════════════════════════
   sv_recv_all  –  grow a DynBuf until the full HTTP request is received
   ══════════════════════════════════════════════════════════════════════ */
static int sv_recv_all(SocketFd client, DynBuf* buf)
{
    char chunk[4096];
    size_t content_length = 0;
    int    headers_done = 0;

    for (;;)
    {
        int n = (int)recv(client, chunk, sizeof(chunk), 0);
        if (n <= 0) break;
        if (sv_db_append(buf, chunk, (size_t)n) != 0) return -1;

        char* sep = strstr(buf->data, "\r\n\r\n");
        if (!sep) continue;   /* headers not complete yet */

        if (!headers_done)
        {
            headers_done = 1;
            char* cl = strstr(buf->data, "Content-Length:");
            if (!cl) cl = strstr(buf->data, "content-length:");
            if (cl) content_length = (size_t)atol(cl + 15);
        }

        size_t header_bytes = (size_t)(sep + 4 - buf->data);
        if (buf->len - header_bytes >= content_length) break;
    }
    return 0;
}


/* ══════════════════════════════════════════════════════════════════════
   sv_get_exe_path  –  platform-specific
   ══════════════════════════════════════════════════════════════════════ */
#if defined(_WIN32)

static char* sv_get_exe_path(char* buffer, size_t size)
{
    DWORD len = GetModuleFileNameA(NULL, buffer, (DWORD)size);
    if (len == 0 || len == size)
        return NULL;
    return buffer;
}

#elif defined(__linux__)
#include <unistd.h>

static char* sv_get_exe_path(char* buffer, size_t size)
{
    ssize_t len = readlink("/proc/self/exe", buffer, size - 1);
    if (len == -1)
        return NULL;
    buffer[len] = '\0';
    return buffer;
}

#elif defined(__APPLE__)
#include <mach-o/dyld.h>

static char* sv_get_exe_path(char* buffer, size_t size)
{
    uint32_t sz = (uint32_t)size;
    if (_NSGetExecutablePath(buffer, &sz) != 0)
        return NULL;  /* buffer too small */
    return buffer;
}

#else
#error "Unsupported platform"
#endif


   /* ══════════════════════════════════════════════════════════════════════
      sv_get_current_path
      ══════════════════════════════════════════════════════════════════════ */
static char* sv_get_current_path(char path[], int sz)
{
#ifdef _WIN32
    if (GetCurrentDirectoryA(sz, path) == 0)
    {
        perror("Erro ao obter diretório atual");
        return NULL;
    }
#else
    if (getcwd(path, (size_t)sz) == NULL)
    {
        perror("Erro ao obter diretório atual");
        return NULL;
    }
#endif
    return path;
}


/* ══════════════════════════════════════════════════════════════════════
   sv_open_browser  –  platform-specific
   ══════════════════════════════════════════════════════════════════════ */
#if defined(_WIN32)

static void sv_open_browser(const char* url)
{
    ShellExecuteA(
        NULL,
        "open",
        "msedge.exe",
        url,
        NULL,
        SW_SHOWNORMAL
    );
}

#elif defined(__APPLE__)

static void sv_open_browser(const char* url)
{
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "open \"%s\"", url);
    system(cmd);
}

#else  /* Linux */

static void sv_open_browser(const char* url)
{
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "xdg-open \"%s\"", url);
    system(cmd);
}

#endif


/* ══════════════════════════════════════════════════════════════════════
   main
   ══════════════════════════════════════════════════════════════════════ */

#include <stdio.h>

static void sv_remove_filename(char* path)
{
    if (!path) return;

    char* last_sep = NULL;

    for (char* p = path; *p; ++p)
    {
        if (*p == '/' || *p == '\\')
        {
            last_sep = p;
        }
    }

    if (last_sep)
    {
        *(last_sep + 1) = '\0'; /* keep the trailing slash */
    }
    else
    {
        path[0] = '\0'; /* no separator → no directory part */
    }
}

static int sv_start(void);

int main(void)
{
    sv_start();
}

static int sv_start(void)
{
    sv_get_current_path(BASE_DIR2, sizeof BASE_DIR2);

    printf("Current dir: %s\n", BASE_DIR2);

    sv_get_exe_path(BASE_DIR, sizeof BASE_DIR);
    printf("Web dir: %s\n", BASE_DIR);

    //#define DEBUG_IDE
#ifdef DEBUG_IDE
    BASE_DIR[strlen(BASE_DIR) - sizeof("Debug\\server.exe")] = 0;
    strcat(BASE_DIR, "\\web");
#else
    sv_remove_filename(BASE_DIR);
#endif

#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

    SocketFd server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) { perror("socket"); return 1; }

    int yes = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR,
        (const char*)&yes, sizeof(yes));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        perror("bind"); return 1;
    }
    listen(server, 10);
    printf("Server running at http://localhost:%d\n", PORT);

#if !defined(DEBUG_IDE) && (defined(_WIN32) || defined(__APPLE__))
    //sv_open_browser("http://localhost:8080/web/playground.html");
#endif

    while (1)
    {
        SocketFd client = accept(server, NULL, NULL);
        if (client == INVALID_SOCKET) continue;

        DynBuf req;
        if (sv_db_init(&req, 4096) != 0) { CLOSE_SOCKET(client); continue; }

        if (sv_recv_all(client, &req) != 0 || req.len == 0)
        {
            sv_db_free(&req); CLOSE_SOCKET(client); continue;
        }

        char method[8] = { 0 }, url[1024] = { 0 };
        sscanf(req.data, "%7s %1023s", method, url);

        char* body = strstr(req.data, "\r\n\r\n");
        size_t body_len = 0;
        if (body)
        {
            body += 4;
            body_len = req.len - (size_t)(body - req.data);
        }

        char* query = strchr(url, '?');
        if (query) *query++ = '\0';

        if (strcmp(method, "GET") == 0)
        {
            if (strcmp(url, "/list") == 0)
                sv_handle_list(client, query ? query : "");
            else if (strcmp(url, "/read") == 0)
                sv_handle_read(client, query ? query : "");
            else
                sv_handle_static(client, url);
        }
        else if (strcmp(method, "POST") == 0)
        {
            if (strcmp(url, "/save") == 0)
                sv_handle_save(client, body ? body : "", body_len);
            else if (strcmp(url, "/compile") == 0)
                sv_handle_compile(client, body ? body : "", body_len);
            else if (strcmp(url, "/build") == 0)
                sv_handle_build(client, body ? body : "", body_len);

            else
                sv_send_str(client, "text/plain", "Not found");
        }
        else
        {
            sv_send_str(client, "text/plain", "Not found");
        }
        sv_db_free(&req);
        CLOSE_SOCKET(client);
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}