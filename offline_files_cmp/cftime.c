/*  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * 2016.11.09   show error message when fail
 *              check file attributes, fail when file is read-only.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

typedef struct {
    FILETIME ctime;
    FILETIME mtime;
    FILETIME atime;
}ftime_t;

void print_error_message(const char* msg, DWORD err)
{
    char message[8196];

    FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, 
            err,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            message,
            sizeof(message),
            NULL);

    printf("%s 0x%08x \"%s\"\n", msg, err, message);
}

void file_print_raw(ftime_t* ft)
{
    printf("raw: 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx\n",
            ft->ctime.dwHighDateTime,
            ft->ctime.dwLowDateTime,
            ft->mtime.dwHighDateTime,
            ft->mtime.dwLowDateTime,
            ft->atime.dwHighDateTime,
            ft->atime.dwLowDateTime);
}

void file_print_time(ftime_t* ft)
{
    FILETIME lft;
    SYSTEMTIME t;

    FileTimeToLocalFileTime(&ft->ctime, &lft);
    FileTimeToSystemTime(&lft, &t);
    printf("created: %04d-%02d-%02d %02d:%02d:%02d\n",
            t.wYear, t.wMonth, t.wDay,
            t.wHour, t.wMinute, t.wSecond);

    FileTimeToLocalFileTime(&ft->mtime, &lft);
    FileTimeToSystemTime(&lft, &t);
    printf("modify : %04d-%02d-%02d %02d:%02d:%02d\n",
            t.wYear, t.wMonth, t.wDay,
            t.wHour, t.wMinute, t.wSecond);

    FileTimeToLocalFileTime(&ft->atime, &lft);
    FileTimeToSystemTime(&lft, &t);
    printf("access : %04d-%02d-%02d %02d:%02d:%02d\n",
            t.wYear, t.wMonth, t.wDay,
            t.wHour, t.wMinute, t.wSecond);
}

int file_get_time(const char* fn, ftime_t* ft)
{
    HANDLE h;

    h = CreateFile(fn,
        FILE_SHARE_READ|FILE_SHARE_WRITE,
        FILE_SHARE_READ,
        NULL, 
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (h == INVALID_HANDLE_VALUE) {
        print_error_message("Error: CreateFile", GetLastError());
        return -1;
    }

    if (!GetFileTime(h, &ft->ctime, &ft->atime, &ft->mtime)) {
        print_error_message("Error: GetFileTime", GetLastError());
        CloseHandle(h);
        return -1;
    }

    CloseHandle(h);
    return 0;
}

int file_put_time(const char* fn, ftime_t* ft)
{
    HANDLE h;
    DWORD attributes;

    attributes = GetFileAttributes(fn);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        print_error_message("Error: GetFileAttributes", GetLastError());
    }
    if (attributes & FILE_ATTRIBUTE_READONLY) {
        printf("Error: Read-only! \"%s\"\n", fn);
        return -1;
    }

    h = CreateFile(fn,
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL, 
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (h == INVALID_HANDLE_VALUE) {
        print_error_message("Error: CreateFile", GetLastError());
        return -1;
    }

    if (!SetFileTime(h, &ft->ctime, &ft->atime, &ft->mtime)) {
        print_error_message("Error: SetFileTime", GetLastError());
        CloseHandle(h);
        return -1;
    }

    CloseHandle(h);
    return 0;
}

int main(int argc, char* argv[])
{
    ftime_t ft;

    if (argc == 2) {
        if (file_get_time(argv[1], &ft) != 0) {
            return -1;
        }
        file_print_raw(&ft);
        file_print_time(&ft);

    } else if (argc == 8) {
        sscanf(argv[2], "0x%lx", &ft.ctime.dwHighDateTime);
        sscanf(argv[3], "0x%lx", &ft.ctime.dwLowDateTime);
        sscanf(argv[4], "0x%lx", &ft.mtime.dwHighDateTime);
        sscanf(argv[5], "0x%lx", &ft.mtime.dwLowDateTime);
        sscanf(argv[6], "0x%lx", &ft.atime.dwHighDateTime);
        sscanf(argv[7], "0x%lx", &ft.atime.dwLowDateTime);
        file_print_time(&ft);
        if (file_put_time(argv[1], &ft) != 0) {
            return -1;
        }

    } else {
        printf("usage (get): %s file\n", argv[0]);
        printf("usage (set): %s file create_h create_l modify_h modify_l access_h access_l\n", argv[0]);
    }

    return 0;
}

