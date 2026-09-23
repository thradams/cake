#pragma once

/*
   Product details, files and events of the Windows installer (win_installer.c).
   This is the only file you need to edit.
   Based on https://github.com/thradams/installer
*/

#include <windows.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include "../version.h"

#define INSTALLER_WIDEN2(s) L##s
#define INSTALLER_WIDEN(s) INSTALLER_WIDEN2(s)

#define INSTALLER_PRODUCT_VERSION   INSTALLER_WIDEN(CAKE_VERSION)
#define INSTALLER_PRODUCT_NAME      L"Cake"
#define INSTALLER_PRODUCT_PUBLISHER L"Cake"
#define INSTALLER_PRODUCT_WEB_SITE  L"https://github.com/thradams/cake"

/* Your product id. Create a new GUID for each product. */
#define INSTALLER_PRODUCT_CODE L"{30D6079B-E24E-40FA-9FC0-42C559191DE1}"

#define INSTALLER_DISPLAY_NAME INSTALLER_PRODUCT_NAME L" " INSTALLER_PRODUCT_VERSION

/* default installation folder, inside Program Files */
#define INSTALLER_INSTALL_SUBDIR L"cake\\" INSTALLER_PRODUCT_VERSION

/*
   {source, destination}
   source is read when setup.exe is built (relative to the current directory):
     - a file           destination is the file path
     - a folder         copied recursively; destination is the folder
     - a wildcard *.h   matching files; destination is the folder
   destination is relative to the installation folder.
   uninstall.exe and uninstall.lst are created by setup.exe.
*/
#define INSTALLER_FILES \
    {L"..\\cake.exe",      L"cake.exe"},    \
    {L"..\\cakeide.exe",   L"cakeide.exe"}, \
    {L"..\\cake.json",     L"cake.json"},   \
    {L"..\\samples",       L"samples"},     \
    {L"..\\include\\*.h",  L"include"},     \
    {L"..\\web",           L"web"}

/* optional: shows "I agree with the license terms"; the file is read when setup.exe is built */
#define INSTALLER_LICENSE_FILE L"win_license.rtf"

/* optional: at the end, asks to run this program (relative to the installation folder) */
#define INSTALLER_RUN_PROGRAM_AT_END L"cakeide.exe"

/* versions are installed side by side, each one has its own uninstall key */
#define INSTALLER_UNINST_KEY L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" INSTALLER_PRODUCT_CODE L"_" INSTALLER_PRODUCT_VERSION

#define ENVIRONMENT_KEY L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"

/* install_dir itself, or when adding, another version: ...\cake\<version> */
static bool is_cake_path_entry(const wchar_t* entry, size_t length, const wchar_t* install_dir, bool add)
{
    if (length == wcslen(install_dir) && _wcsnicmp(entry, install_dir, length) == 0)
        return true;

    if (!add || length >= MAX_PATH)
        return false;

    wchar_t path[MAX_PATH];
    wmemcpy(path, entry, length);
    path[length] = 0;

    wchar_t* last = wcsrchr(path, L'\\');
    if (last == NULL)
        return false;
    *last = 0;

    wchar_t* parent = wcsrchr(path, L'\\');
    return parent && _wcsicmp(parent + 1, L"cake") == 0;
}

/* Removes install_dir (and old cake versions when adding) from the system
   PATH, then appends install_dir if add is true. */
static void update_system_path(const wchar_t* install_dir, bool add)
{
    HKEY key;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, ENVIRONMENT_KEY, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &key) != ERROR_SUCCESS)
        return;

    DWORD type = REG_EXPAND_SZ;
    DWORD size = 0;
    if (RegQueryValueExW(key, L"Path", NULL, &type, NULL, &size) == ERROR_SUCCESS)
    {
        const size_t capacity = size / sizeof(wchar_t) + wcslen(install_dir) + 2;
        wchar_t* old_path = calloc(size / sizeof(wchar_t) + 1, sizeof(wchar_t));
        wchar_t* new_path = calloc(capacity, sizeof(wchar_t));

        if (old_path && new_path &&
            RegQueryValueExW(key, L"Path", NULL, &type, (BYTE*)old_path, &size) == ERROR_SUCCESS)
        {
            const wchar_t* p = old_path;
            for (;;)
            {
                const wchar_t* end = wcschr(p, L';');
                const size_t length = end ? (size_t)(end - p) : wcslen(p);
                if (length > 0 && !is_cake_path_entry(p, length, install_dir, add))
                {
                    if (new_path[0])
                        wcscat_s(new_path, capacity, L";");
                    wcsncat_s(new_path, capacity, p, length);
                }
                if (end == NULL)
                    break;
                p = end + 1;
            }

            if (add)
            {
                if (new_path[0])
                    wcscat_s(new_path, capacity, L";");
                wcscat_s(new_path, capacity, install_dir);
            }

            if (RegSetValueExW(key, L"Path", 0, type, (const BYTE*)new_path, (DWORD)((wcslen(new_path) + 1) * sizeof(wchar_t))) == ERROR_SUCCESS)
                SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 1000, NULL);
        }
        free(old_path);
        free(new_path);
    }
    RegCloseKey(key);
}

/*
   Called after all files and the uninstall registry key were written.
*/
static void on_installed(const wchar_t* install_dir)
{
    update_system_path(install_dir, true);
}

/*
   Called before the files are removed.
*/
static void on_uninstall(const wchar_t* install_dir)
{
    update_system_path(install_dir, false);
}
