/*
   Minimal Windows installer.
   Based on https://github.com/thradams/installer

   Files:
     win_installer.h    product details, files and events
     win_installer.c    this file
     win_installer.ico  icon of setup.exe and uninstall.exe
     win_license.rtf    license shown by setup.exe

   Build:
     cl win_installer.c
     win_installer.exe setup.exe

   Note: Windows asks elevation for 32-bit exes without manifest whose name
   has "install" or "setup" (installer detection). To avoid it, build with
   another name, e.g. cl win_installer.c -Fe:win_pack.exe

   win_installer.exe setup.exe
     creates setup.exe: a copy of win_installer.exe with the INSTALLER_FILES (compressed),
     license, icon and manifest (requireAdministrator) added as resources.

   setup.exe
     shows the setup dialog and installs the files, uninstall.exe (setup.exe
     without the files), uninstall.lst (installed files) and the uninstall
     registry key.

   uninstall.exe
     removes the files listed in uninstall.lst, the registry key and the folder.

   Configuration: INSTALLER_ macros in win_installer.h (optional ones are
   checked with #ifdef).

   Debugging (Developer Command Prompt started as Administrator, because
   setup.exe and uninstall.exe require it):
     cl /Zi win_installer.c
     devenv /DebugExe win_installer.exe setup.exe     (debug the build step)
     win_installer.exe setup.exe
     devenv /DebugExe setup.exe                       (debug the installer)
     devenv /DebugExe "C:\Program Files\...\uninstall.exe"  (debug the uninstaller)
   setup.exe and uninstall.exe are copies of win_installer.exe, so
   win_installer.pdb is used for both.
*/

#ifndef UNICODE
#define UNICODE
#endif
#include "win_installer.h"

#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <richedit.h>
#include <compressapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "cabinet.lib")

#define RES_PAYLOAD L"PAYLOAD"
#define RES_LICENSE L"LICENSE"
#define RES_UNINSTALL L"UNINSTALL" /* marks uninstall.exe */

struct install_file
{
    const wchar_t* source;
    const wchar_t* destination;
};

static const struct install_file files[] = { INSTALLER_FILES };

static const char manifest[] =
"<assembly xmlns='urn:schemas-microsoft-com:asm.v1' manifestVersion='1.0'>"
"<trustInfo xmlns='urn:schemas-microsoft-com:asm.v3'><security><requestedPrivileges>"
"<requestedExecutionLevel level='requireAdministrator' uiAccess='false'/>"
"</requestedPrivileges></security></trustInfo>"
"<dependency><dependentAssembly>"
"<assemblyIdentity type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0'"
" processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'/>"
"</dependentAssembly></dependency>"
"<application xmlns='urn:schemas-microsoft-com:asm.v3'><windowsSettings>"
"<dpiAware xmlns='http://schemas.microsoft.com/SMI/2005/WindowsSettings'>true</dpiAware>"
"</windowsSettings></application>"
"</assembly>";

struct buffer
{
    unsigned char* data;
    size_t size;
    size_t capacity;
};

static bool buffer_append(struct buffer* b, const void* data, size_t size)
{
    if (b->size + size > b->capacity)
    {
        size_t capacity = b->capacity * 2 + size;
        unsigned char* p = realloc(b->data, capacity);
        if (p == NULL)
            return false;
        b->data = p;
        b->capacity = capacity;
    }
    if (size > 0)
        memcpy(b->data + b->size, data, size);
    b->size += size;
    return true;
}

static bool read_file(const wchar_t* path, struct buffer* b)
{
    HANDLE h = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE)
        return false;

    LARGE_INTEGER size;
    DWORD n = 0;
    unsigned char* p = NULL;
    if (GetFileSizeEx(h, &size) && size.QuadPart < 0x7FFFFFFF)
        p = realloc(b->data, b->size + (size_t)size.QuadPart + sizeof(wchar_t));

    bool ok = p != NULL;
    if (ok)
    {
        b->data = p;
        b->capacity = b->size + (size_t)size.QuadPart + sizeof(wchar_t);
        ok = ReadFile(h, p + b->size, (DWORD)size.QuadPart, &n, NULL) && n == size.QuadPart;
        b->size += n;
    }
    CloseHandle(h);
    return ok;
}

static bool write_file(const wchar_t* path, const void* data, size_t size)
{
    HANDLE h = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (h == INVALID_HANDLE_VALUE)
        return false;
    DWORD n = 0;
    bool ok = WriteFile(h, data, (DWORD)size, &n, NULL) && n == size;
    CloseHandle(h);
    return ok;
}

/*
   -------------------------------------------------------------------------
   win_installer.exe setup.exe

   Payload (compressed with LZMS):
     u32 count
     count x { u32 name_size, wchar_t name[], u32 data_size, data[] }
   -------------------------------------------------------------------------
*/

static bool add_file(struct buffer* payload, unsigned int* count, const wchar_t* source, const wchar_t* name)
{
    struct buffer data = { 0 };
    if (!read_file(source, &data))
    {
        fwprintf(stderr, L"error: cannot read '%ls'\n", source);
        free(data.data);
        return false;
    }

    unsigned int name_size = (unsigned int)(wcslen(name) * sizeof(wchar_t));
    unsigned int data_size = (unsigned int)data.size;
    bool ok = buffer_append(payload, &name_size, sizeof name_size) &&
              buffer_append(payload, name, name_size) &&
              buffer_append(payload, &data_size, sizeof data_size) &&
              buffer_append(payload, data.data, data.size);
    free(data.data);
    (*count)++;
    return ok;
}

/* files of dir matching pattern; subfolders too if recursive */
static bool add_dir(struct buffer* payload, unsigned int* count,
                    const wchar_t* dir, const wchar_t* pattern,
                    const wchar_t* name, bool recursive)
{
    wchar_t search[MAX_PATH];
    swprintf(search, MAX_PATH, L"%ls\\%ls", dir, pattern);

    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(search, &fd);
    if (h == INVALID_HANDLE_VALUE)
    {
        fwprintf(stderr, L"error: nothing matches '%ls'\n", search);
        return false;
    }

    bool ok = true;
    do
    {
        wchar_t child_source[MAX_PATH], child_name[MAX_PATH];
        swprintf(child_source, MAX_PATH, L"%ls\\%ls", dir, fd.cFileName);
        swprintf(child_name, MAX_PATH, L"%ls\\%ls", name, fd.cFileName);

        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            ok = add_file(payload, count, child_source, child_name);
        else if (recursive && fd.cFileName[0] != L'.')
            ok = add_dir(payload, count, child_source, L"*", child_name, true);
    }
    while (ok && FindNextFileW(h, &fd));

    FindClose(h);
    return ok;
}

/* source is a file, a folder (recursive) or a wildcard like dir\*.h */
static bool add_source(struct buffer* payload, unsigned int* count, const wchar_t* source, const wchar_t* name)
{
    const wchar_t* pattern = wcsrchr(source, L'\\');
    if (pattern && wcspbrk(pattern, L"*?"))
    {
        wchar_t dir[MAX_PATH];
        swprintf(dir, MAX_PATH, L"%.*ls", (int)(pattern - source), source);
        return add_dir(payload, count, dir, pattern + 1, name, false);
    }

    const DWORD attributes = GetFileAttributesW(source);
    if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY))
        return add_dir(payload, count, source, L"*", name, true);

    return add_file(payload, count, source, name);
}

static bool compress_buffer(const struct buffer* in, struct buffer* out)
{
    COMPRESSOR_HANDLE compressor;
    if (!CreateCompressor(COMPRESS_ALGORITHM_LZMS, NULL, &compressor))
        return false;

    SIZE_T size = 0;
    Compress(compressor, in->data, in->size, NULL, 0, &size);
    out->data = malloc(size);
    bool ok = out->data && Compress(compressor, in->data, in->size, out->data, size, &size);
    out->size = size;
    CloseCompressor(compressor);
    return ok;
}

/* .ico file -> RT_ICON + RT_GROUP_ICON resources */
static bool update_icon(HANDLE update, const struct buffer* ico)
{
#pragma pack(push, 2)
    struct icon_entry { BYTE width, height, colors, reserved; WORD planes, bit_count; DWORD bytes, offset; };
    struct group_entry { BYTE width, height, colors, reserved; WORD planes, bit_count; DWORD bytes; WORD id; };
#pragma pack(pop)

    if (ico->size < 6)
        return false;

    WORD count;
    memcpy(&count, ico->data + 4, sizeof count);

    struct buffer group = { 0 };
    bool ok = 6 + count * sizeof(struct icon_entry) <= ico->size && buffer_append(&group, ico->data, 6);
    for (WORD i = 0; ok && i < count; i++)
    {
        struct icon_entry e;
        memcpy(&e, ico->data + 6 + i * sizeof e, sizeof e);

        struct group_entry g;
        memcpy(&g, &e, offsetof(struct group_entry, id));
        g.id = i + 1;

        ok = (size_t)e.offset + e.bytes <= ico->size &&
             UpdateResourceW(update, RT_ICON, MAKEINTRESOURCEW(i + 1), 0, ico->data + e.offset, e.bytes) &&
             buffer_append(&group, &g, sizeof g);
    }
    ok = ok && UpdateResourceW(update, RT_GROUP_ICON, MAKEINTRESOURCEW(1), 0, group.data, (DWORD)group.size);
    free(group.data);
    return ok;
}

/* setup.exe must not open a console window */
static bool set_gui_subsystem(struct buffer* exe)
{
    IMAGE_DOS_HEADER dos;
    memcpy(&dos, exe->data, sizeof dos);
    size_t offset = dos.e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + offsetof(IMAGE_OPTIONAL_HEADER, Subsystem);
    if (dos.e_magic != IMAGE_DOS_SIGNATURE || offset + sizeof(WORD) > exe->size)
        return false;

    WORD subsystem = IMAGE_SUBSYSTEM_WINDOWS_GUI;
    memcpy(exe->data + offset, &subsystem, sizeof subsystem);
    return true;
}

static int build(const wchar_t* output)
{
    wchar_t self[MAX_PATH];
    struct buffer exe = { 0 }, icon = { 0 }, license = { 0 }, payload = { 0 }, compressed = { 0 };
    unsigned int count = 0;

    bool ok = GetModuleFileNameW(NULL, self, MAX_PATH) && read_file(self, &exe) && set_gui_subsystem(&exe);
    if (ok && !read_file(L"win_installer.ico", &icon))
    {
        fwprintf(stderr, L"error: cannot read 'win_installer.ico'\n");
        ok = false;
    }
#ifdef INSTALLER_LICENSE_FILE
    if (ok && !(read_file(INSTALLER_LICENSE_FILE, &license) && buffer_append(&license, "", 1)))
    {
        fwprintf(stderr, L"error: cannot read '%ls'\n", INSTALLER_LICENSE_FILE);
        ok = false;
    }
#endif

    ok = ok && buffer_append(&payload, &count, sizeof count); /* updated below */
    for (int i = 0; ok && i < (int)(sizeof files / sizeof files[0]); i++)
    {
        unsigned int before = count;
        ok = add_source(&payload, &count, files[i].source, files[i].destination);
        wprintf(L"  %ls (%u files)\n", files[i].source, count - before);
    }
    if (ok)
        memcpy(payload.data, &count, sizeof count);

    ok = ok && compress_buffer(&payload, &compressed) && write_file(output, exe.data, exe.size);

    HANDLE update = ok ? BeginUpdateResourceW(output, FALSE) : NULL;
    ok = update &&
         UpdateResourceW(update, RT_MANIFEST, MAKEINTRESOURCEW(1), 0, (void*)manifest, sizeof manifest - 1) &&
         update_icon(update, &icon) &&
         (license.size == 0 || UpdateResourceW(update, RT_RCDATA, RES_LICENSE, 0, license.data, (DWORD)license.size)) &&
         UpdateResourceW(update, RT_RCDATA, RES_PAYLOAD, 0, compressed.data, (DWORD)compressed.size);
    ok = update && EndUpdateResourceW(update, !ok) && ok;

    if (ok)
        wprintf(L"%ls created (%zu bytes compressed to %zu)\n", output, payload.size, compressed.size);
    else
        fwprintf(stderr, L"error: cannot create '%ls'\n", output);

    free(exe.data);
    free(icon.data);
    free(license.data);
    free(payload.data);
    free(compressed.data);
    return ok ? 0 : 1;
}

/*
   -------------------------------------------------------------------------
   setup.exe
   -------------------------------------------------------------------------
*/

static void make_path(wchar_t path[MAX_PATH], const wchar_t* dir, const wchar_t* name)
{
    swprintf(path, MAX_PATH, L"%ls\\%ls", dir, name);
}

static bool set_string(HKEY key, const wchar_t* name, const wchar_t* value)
{
    return RegSetValueExW(key, name, 0, REG_SZ, (const BYTE*)value, (DWORD)((wcslen(value) + 1) * sizeof(wchar_t))) == ERROR_SUCCESS;
}

/* https://learn.microsoft.com/en-us/windows/win32/msi/uninstall-registry-key */
static bool write_uninstall_key(const wchar_t* install_dir)
{
    wchar_t uninstall[MAX_PATH + 20], icon[MAX_PATH + 20];
    swprintf(uninstall, MAX_PATH + 20, L"\"%ls\\uninstall.exe\"", install_dir);
    swprintf(icon, MAX_PATH + 20, L"%ls\\uninstall.exe,0", install_dir);

    HKEY key;
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, INSTALLER_UNINST_KEY, 0, NULL, 0, KEY_WRITE, NULL, &key, NULL) != ERROR_SUCCESS)
        return false;

    bool ok = set_string(key, L"DisplayName", INSTALLER_DISPLAY_NAME) &&
              set_string(key, L"DisplayVersion", INSTALLER_PRODUCT_VERSION) &&
              set_string(key, L"Publisher", INSTALLER_PRODUCT_PUBLISHER) &&
              set_string(key, L"URLInfoAbout", INSTALLER_PRODUCT_WEB_SITE) &&
              set_string(key, L"InstallLocation", install_dir) &&
              set_string(key, L"UninstallString", uninstall) &&
              set_string(key, L"DisplayIcon", icon);
    RegCloseKey(key);
    return ok;
}

/* uninstall.exe is setup.exe without payload and license, marked with RES_UNINSTALL */
static bool write_uninstaller(const wchar_t* install_dir)
{
    wchar_t self[MAX_PATH], path[MAX_PATH];
    make_path(path, install_dir, L"uninstall.exe");
    if (!GetModuleFileNameW(NULL, self, MAX_PATH) || !CopyFileW(self, path, FALSE))
        return false;

    HANDLE update = BeginUpdateResourceW(path, FALSE);
    bool ok = update &&
              UpdateResourceW(update, RT_RCDATA, RES_PAYLOAD, 0, NULL, 0) &&
#ifdef INSTALLER_LICENSE_FILE
              UpdateResourceW(update, RT_RCDATA, RES_LICENSE, 0, NULL, 0) &&
#endif
              UpdateResourceW(update, RT_RCDATA, RES_UNINSTALL, 0, "1", 1);
    return update && EndUpdateResourceW(update, !ok) && ok;
}

enum
{
    IDC_APP_ICON = 100,
    IDC_PRODUCT,
    IDC_PUBLISHER,
    IDC_DEST_FRAME,
    IDC_DESTINATION,
    IDC_BROWSE,
    IDC_AGREE,
    IDC_LICENSE_LINK,
    IDC_MESSAGE,
    IDC_PROGRESS,
    IDC_INSTALL,
    IDC_LICENSE_TEXT
};

/* extracts the payload; the names are also written to uninstall.lst */
static bool extract(HWND dialog, const wchar_t* install_dir, const void* compressed, DWORD compressed_size)
{
    DECOMPRESSOR_HANDLE decompressor;
    if (!CreateDecompressor(COMPRESS_ALGORITHM_LZMS, NULL, &decompressor))
        return false;

    SIZE_T size = 0;
    Decompress(decompressor, compressed, compressed_size, NULL, 0, &size);
    unsigned char* data = malloc(size);
    bool ok = data && Decompress(decompressor, compressed, compressed_size, data, size, &size);
    CloseDecompressor(decompressor);

    struct buffer list = { 0 };
    const unsigned char* p = data;
    unsigned int count = 0;
    if (ok)
    {
        memcpy(&count, p, sizeof count);
        p += sizeof count;
    }
    SendDlgItemMessageW(dialog, IDC_PROGRESS, PBM_SETRANGE32, 0, count);

    for (unsigned int i = 0; ok && i < count; i++)
    {
        unsigned int name_size, data_size;
        wchar_t name[MAX_PATH] = { 0 }, path[MAX_PATH], dir[MAX_PATH];

        memcpy(&name_size, p, sizeof name_size);
        p += sizeof name_size;
        memcpy(name, p, name_size < sizeof name ? name_size : sizeof name - sizeof(wchar_t));
        p += name_size;
        memcpy(&data_size, p, sizeof data_size);
        p += sizeof data_size;

        wchar_t message[MAX_PATH + 50];
        swprintf(message, MAX_PATH + 50, L"Extracting %ls (%u of %u)", name, i + 1, count);
        SetDlgItemTextW(dialog, IDC_MESSAGE, message);
        SendDlgItemMessageW(dialog, IDC_PROGRESS, PBM_SETPOS, i + 1, 0);
        RedrawWindow(dialog, NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN);

        make_path(path, install_dir, name);
        wcscpy_s(dir, MAX_PATH, path);
        *wcsrchr(dir, L'\\') = 0;
        SHCreateDirectoryExW(NULL, dir, NULL);

        ok = write_file(path, p, data_size) &&
             buffer_append(&list, name, name_size) &&
             buffer_append(&list, L"\n", sizeof(wchar_t));
        p += data_size;
    }

    wchar_t list_path[MAX_PATH];
    make_path(list_path, install_dir, L"uninstall.lst");
    ok = ok && write_file(list_path, list.data, list.size);

    free(list.data);
    free(data);
    return ok;
}

struct setup_dialog
{
    HWND hwnd;
    const void* payload;
    DWORD payload_size;
    const char* license;
};

/*
   In-memory dialog templates (DLGTEMPLATE + DLGITEMTEMPLATE).
*/

static WORD* template_string(WORD* p, const wchar_t* s)
{
    while ((*p++ = *s++) != 0) {}
    return p;
}

static WORD* template_begin(DLGTEMPLATE* t, short cx, short cy)
{
    t->style = WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_CENTER | DS_SHELLFONT;
    t->dwExtendedStyle = 0;
    t->cdit = 0;
    t->x = 0;
    t->y = 0;
    t->cx = cx;
    t->cy = cy;

    /* same font as message boxes (e.g. Segoe UI 9) */
    NONCLIENTMETRICSW metrics = { sizeof metrics };
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof metrics, &metrics, 0);
    HDC hdc = GetDC(NULL);
    int point_size = -MulDiv(metrics.lfMessageFont.lfHeight, 72, GetDeviceCaps(hdc, LOGPIXELSY));
    ReleaseDC(NULL, hdc);

    WORD* p = (WORD*)(t + 1);
    *p++ = 0; /* no menu */
    *p++ = 0; /* default class */
    p = template_string(p, L"");
    *p++ = (WORD)(point_size > 0 ? point_size : 8);
    return template_string(p, metrics.lfMessageFont.lfFaceName[0] ? metrics.lfMessageFont.lfFaceName : L"MS Shell Dlg");
}

static WORD* template_item(DLGTEMPLATE* t, WORD* p,
                           const wchar_t* class_name, DWORD style,
                           short x, short y, short cx, short cy,
                           WORD id, const wchar_t* text)
{
    p = (WORD*)(((ULONG_PTR)p + 3) & ~(ULONG_PTR)3);

    DLGITEMTEMPLATE* item = (DLGITEMTEMPLATE*)p;
    item->style = style | WS_CHILD | WS_VISIBLE;
    item->dwExtendedStyle = 0;
    item->x = x;
    item->y = y;
    item->cx = cx;
    item->cy = cy;
    item->id = id;

    p = (WORD*)(item + 1);
    p = template_string(p, class_name);
    p = template_string(p, text);
    *p++ = 0; /* no creation data */
    t->cdit++;
    return p;
}

static INT_PTR CALLBACK license_dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        SETTEXTEX st = { ST_DEFAULT, CP_ACP };
        SetWindowTextW(hwnd, L"License");
        SendDlgItemMessageW(hwnd, IDC_LICENSE_TEXT, EM_SETTEXTEX, (WPARAM)&st, lparam);
        return TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wparam) == IDOK || LOWORD(wparam) == IDCANCEL)
        {
            EndDialog(hwnd, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

static void show_license(struct setup_dialog* p)
{
    DWORD storage[512];
    DLGTEMPLATE* t = (DLGTEMPLATE*)storage;
    WORD* w = template_begin(t, 310, 177);
    w = template_item(t, w, L"RichEdit20W", ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_BORDER | WS_VSCROLL,
                      7, 7, 296, 141, IDC_LICENSE_TEXT, L"");
    w = template_item(t, w, L"Button", BS_DEFPUSHBUTTON | WS_TABSTOP, 253, 156, 50, 14, IDOK, L"Close");

    DialogBoxIndirectParamW(GetModuleHandleW(NULL), t, p->hwnd, license_dialog_proc, (LPARAM)p->license);
}

static void show_page(struct setup_dialog* p, int page)
{
    static const int page0[] = { IDC_DEST_FRAME, IDC_DESTINATION, IDC_BROWSE, IDC_AGREE, IDC_LICENSE_LINK };
    for (int i = 0; i < (int)(sizeof page0 / sizeof page0[0]); i++)
        ShowWindow(GetDlgItem(p->hwnd, page0[i]), page == 0 ? SW_SHOW : SW_HIDE);

    ShowWindow(GetDlgItem(p->hwnd, IDC_PROGRESS), page == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(p->hwnd, IDC_MESSAGE), page != 0 ? SW_SHOW : SW_HIDE);

    if (page == 2)
    {
        ShowWindow(GetDlgItem(p->hwnd, IDC_INSTALL), SW_HIDE);
        SetDlgItemTextW(p->hwnd, IDCANCEL, L"Close");
    }
}

static void install(struct setup_dialog* p)
{
    wchar_t install_dir[MAX_PATH];
    GetDlgItemTextW(p->hwnd, IDC_DESTINATION, install_dir, MAX_PATH);

    show_page(p, 1);
    EnableWindow(GetDlgItem(p->hwnd, IDC_INSTALL), FALSE);
    EnableWindow(GetDlgItem(p->hwnd, IDCANCEL), FALSE);

    const wchar_t* error = NULL;
    if (!extract(p->hwnd, install_dir, p->payload, p->payload_size))
        error = L"Failed to extract the installation files.";
    else if (!write_uninstaller(install_dir))
        error = L"Failed to write uninstall.exe.";
    else if (!write_uninstall_key(install_dir))
        error = L"Failed to write the registry keys.";

    EnableWindow(GetDlgItem(p->hwnd, IDCANCEL), TRUE);

    if (error)
    {
        MessageBoxW(p->hwnd, error, INSTALLER_DISPLAY_NAME, MB_ICONERROR | MB_OK);
        EnableWindow(GetDlgItem(p->hwnd, IDC_INSTALL), TRUE);
        show_page(p, 0);
        return;
    }

    on_installed(install_dir);

    show_page(p, 2);
    SetDlgItemTextW(p->hwnd, IDC_MESSAGE, INSTALLER_DISPLAY_NAME L" was installed successfully.");

#ifdef INSTALLER_RUN_PROGRAM_AT_END
    wchar_t program[MAX_PATH];
    make_path(program, install_dir, INSTALLER_RUN_PROGRAM_AT_END);
    if (MessageBoxW(p->hwnd, L"Do you want to run " INSTALLER_RUN_PROGRAM_AT_END L" now?", INSTALLER_DISPLAY_NAME, MB_ICONQUESTION | MB_YESNO) == IDYES)
        ShellExecuteW(NULL, L"open", program, NULL, install_dir, SW_SHOWNORMAL);
#endif
}

static void browse_folder(struct setup_dialog* p)
{
    BROWSEINFOW bi = { 0 };
    bi.hwndOwner = p->hwnd;
    bi.lpszTitle = L"Select the destination folder";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    PIDLIST_ABSOLUTE pidl = SHBrowseForFolderW(&bi);
    if (pidl)
    {
        wchar_t path[MAX_PATH];
        if (SHGetPathFromIDListW(pidl, path))
            SetDlgItemTextW(p->hwnd, IDC_DESTINATION, path);
        CoTaskMemFree(pidl);
    }
}

static void setup_dialog_init(struct setup_dialog* p)
{
    HICON icon = LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(1));
    SendMessageW(p->hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
    SendMessageW(p->hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
    SendDlgItemMessageW(p->hwnd, IDC_APP_ICON, STM_SETICON, (WPARAM)icon, 0);

    SetWindowTextW(p->hwnd, INSTALLER_DISPLAY_NAME L" Setup");
    SetDlgItemTextW(p->hwnd, IDC_PRODUCT, INSTALLER_DISPLAY_NAME);
    SetDlgItemTextW(p->hwnd, IDC_PUBLISHER, INSTALLER_PRODUCT_PUBLISHER);

    wchar_t program_files[MAX_PATH], destination[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_PROGRAM_FILES, NULL, 0, program_files);
    make_path(destination, program_files, INSTALLER_INSTALL_SUBDIR);
    SetDlgItemTextW(p->hwnd, IDC_DESTINATION, destination);

#ifdef INSTALLER_LICENSE_FILE
    /* user must agree with the license */
    EnableWindow(GetDlgItem(p->hwnd, IDC_INSTALL), FALSE);
#endif

    show_page(p, 0);
}

static INT_PTR CALLBACK setup_dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    struct setup_dialog* p = (struct setup_dialog*)GetWindowLongPtrW(hwnd, DWLP_USER);

    switch (message)
    {
    case WM_INITDIALOG:
        p = (struct setup_dialog*)lparam;
        SetWindowLongPtrW(hwnd, DWLP_USER, (LONG_PTR)p);
        p->hwnd = hwnd;
        setup_dialog_init(p);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wparam))
        {
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        case IDC_AGREE:
            EnableWindow(GetDlgItem(hwnd, IDC_INSTALL), IsDlgButtonChecked(hwnd, IDC_AGREE) == BST_CHECKED);
            break;
        case IDC_BROWSE:
            browse_folder(p);
            break;
        case IDC_INSTALL:
            if (IsWindowEnabled(GetDlgItem(hwnd, IDC_INSTALL)))
                install(p);
            break;
        }
        return TRUE;

    case WM_NOTIFY:
    {
        const NMHDR* nm = (const NMHDR*)lparam;
        if (nm->idFrom == IDC_LICENSE_LINK && (nm->code == NM_CLICK || nm->code == NM_RETURN))
        {
            show_license(p);
            return TRUE;
        }
        break;
    }
    }
    return FALSE;
}

static int setup(const void* payload, DWORD payload_size)
{
    struct setup_dialog dialog = { 0 };
    dialog.payload = payload;
    dialog.payload_size = payload_size;

    HRSRC resource = FindResourceW(NULL, RES_LICENSE, RT_RCDATA);
    dialog.license = resource ? LockResource(LoadResource(NULL, resource)) : "";

    INITCOMMONCONTROLSEX icc = { sizeof icc, ICC_STANDARD_CLASSES | ICC_LINK_CLASS | ICC_PROGRESS_CLASS };
    InitCommonControlsEx(&icc);
    LoadLibraryW(L"Riched20.dll");
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    DWORD storage[1024];
    DLGTEMPLATE* t = (DLGTEMPLATE*)storage;
    WORD* w = template_begin(t, 279, 120);
    w = template_item(t, w, L"Static", SS_ICON, 14, 14, 20, 20, IDC_APP_ICON, L"");
    w = template_item(t, w, L"Static", SS_NOPREFIX, 42, 14, 220, 8, IDC_PRODUCT, L"");
    w = template_item(t, w, L"Static", SS_NOPREFIX, 42, 26, 220, 8, IDC_PUBLISHER, L"");
    w = template_item(t, w, L"Button", BS_GROUPBOX, 13, 44, 257, 32, IDC_DEST_FRAME, L"Destination folder");
    w = template_item(t, w, L"Edit", ES_AUTOHSCROLL | WS_BORDER | WS_TABSTOP, 20, 57, 224, 12, IDC_DESTINATION, L"");
    w = template_item(t, w, L"Button", BS_PUSHBUTTON | WS_TABSTOP, 247, 56, 18, 14, IDC_BROWSE, L"...");
#ifdef INSTALLER_LICENSE_FILE
    w = template_item(t, w, L"Button", BS_AUTOCHECKBOX | WS_TABSTOP, 14, 82, 62, 12, IDC_AGREE, L"I agree with the");
    w = template_item(t, w, L"SysLink", WS_TABSTOP, 77, 84, 80, 10, IDC_LICENSE_LINK, L"<a>license terms</a>");
#endif
    w = template_item(t, w, L"Static", SS_NOPREFIX, 14, 44, 256, 20, IDC_MESSAGE, L"");
    w = template_item(t, w, L"msctls_progress32", WS_BORDER, 14, 66, 256, 12, IDC_PROGRESS, L"");
    w = template_item(t, w, L"Button", BS_DEFPUSHBUTTON | WS_TABSTOP, 164, 100, 52, 14, IDC_INSTALL, L"Install");
    w = template_item(t, w, L"Button", BS_PUSHBUTTON | WS_TABSTOP, 220, 100, 52, 14, IDCANCEL, L"Cancel");

    DialogBoxIndirectParamW(GetModuleHandleW(NULL), t, NULL, setup_dialog_proc, (LPARAM)&dialog);

    CoUninitialize();
    return 0;
}

/*
   -------------------------------------------------------------------------
   uninstall.exe /uninstall
   -------------------------------------------------------------------------
*/

static int uninstall(void)
{
    if (MessageBoxW(NULL, L"Remove " INSTALLER_DISPLAY_NAME L"?", INSTALLER_DISPLAY_NAME L" Uninstall", MB_ICONQUESTION | MB_YESNO) != IDYES)
        return 1;

    wchar_t install_dir[MAX_PATH], path[MAX_PATH];
    GetModuleFileNameW(NULL, install_dir, MAX_PATH);
    *wcsrchr(install_dir, L'\\') = 0;

    on_uninstall(install_dir);

    struct buffer list = { 0 };
    make_path(path, install_dir, L"uninstall.lst");
    bool ok = read_file(path, &list) && buffer_append(&list, L"", sizeof(wchar_t));
    DeleteFileW(path);

    /* delete the files, then their folders if empty */
    for (int pass = 0; ok && pass < 2; pass++)
    {
        wchar_t* context = NULL;
        wchar_t* names = _wcsdup((const wchar_t*)list.data);
        for (wchar_t* name = wcstok_s(names, L"\n", &context); name; name = wcstok_s(NULL, L"\n", &context))
        {
            make_path(path, install_dir, name);
            if (pass == 0)
            {
                DeleteFileW(path);
                continue;
            }
            for (wchar_t* slash; (slash = wcsrchr(path, L'\\')) && slash - path > (ptrdiff_t)wcslen(install_dir);)
            {
                *slash = 0;
                if (!RemoveDirectoryW(path))
                    break;
            }
        }
        free(names);
    }
    free(list.data);

    RegDeleteTreeW(HKEY_LOCAL_MACHINE, INSTALLER_UNINST_KEY);
    RegDeleteKeyW(HKEY_LOCAL_MACHINE, INSTALLER_UNINST_KEY);

    MessageBoxW(NULL,
                ok ? INSTALLER_DISPLAY_NAME L" was removed." : L"Cannot read uninstall.lst.",
                INSTALLER_DISPLAY_NAME L" Uninstall",
                ok ? MB_ICONINFORMATION : MB_ICONERROR);

    /* a running exe cannot delete itself: cmd removes uninstall.exe and the folder */
    wchar_t command[3 * MAX_PATH];
    swprintf(command, 3 * MAX_PATH,
             L"cmd.exe /c ping -n 3 127.0.0.1 > nul & del \"%ls\\uninstall.exe\" & rmdir \"%ls\"",
             install_dir, install_dir);
    wchar_t temp_dir[MAX_PATH]; /* cmd must not run inside install_dir */
    GetTempPathW(MAX_PATH, temp_dir);
    STARTUPINFOW si = { sizeof si };
    PROCESS_INFORMATION pi;
    if (CreateProcessW(NULL, command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, temp_dir, &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    return ok ? 0 : 1;
}

int wmain(int argc, wchar_t* argv[])
{
    if (FindResourceW(NULL, RES_UNINSTALL, RT_RCDATA))
        return uninstall();

    HRSRC resource = FindResourceW(NULL, RES_PAYLOAD, RT_RCDATA);
    if (resource)
        return setup(LockResource(LoadResource(NULL, resource)), SizeofResource(NULL, resource));

    if (argc != 2)
    {
        wprintf(L"usage: win_installer.exe setup.exe\n");
        return 1;
    }
    return build(argv[1]);
}
