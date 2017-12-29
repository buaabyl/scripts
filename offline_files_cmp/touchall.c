/* vim: set fileencoding=utf-8:
 *
 *                   GNU GENERAL PUBLIC LICENSE
 *                       Version 2, June 1991
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 *
 *  2017.06.01     created by buaa.byl@gmail.com
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <windows.h>

const char* GetLastErrorMessage(void)
{
    DWORD err;
    int n;
    static char message[8196];

    err = GetLastError();
    n = sprintf(message, "Error 0x%08x: ", err);

    FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, 
            err,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            message + n,
            sizeof(message) - n,
            NULL);

    return message;
}

int main(int argc, char* argv[])
{
    HANDLE h;
    SYSTEMTIME st;
    FILETIME ft;
    int i;

    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &ft);

    //https://msdn.microsoft.com/en-us/library/windows/desktop/aa363874(v=vs.85).aspx
    for (i = 1;i < argc;i++) {
        h = CreateFile(argv[i],
            GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL, 
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if (h == INVALID_HANDLE_VALUE) {
            printf("CreateFile(%s): %s\n", argv[i], GetLastErrorMessage());
            continue;
        }
        if (!SetFileTime(h, &ft, &ft, &ft)) {
            printf("SetFileTime(%s): %s\n", argv[i], GetLastErrorMessage());
        }
        CloseHandle(h);

        printf("M %s\n", argv[i]);
    }

    return 0;
}

