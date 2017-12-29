/*  
 *  Copyright 2013 buaa.byl@gmail.com
 *
 *
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_EVENTS 15
static int g_max_events = MAX_EVENTS;
static SYSTEMTIME g_prev_st;

const char* get_code_name(DWORD event_code)
{
    switch(event_code) {
    //Microsoft-Windows-Kernel-General
    case 12: return "System Startup";
    case 13: return "System Shutdown";
    //Microsoft-Windows-Kernel-Power
    case 41: return "ACPI Stoped/Crashed/Lost power";
    case 42: return "ACPI Sleep";
    case 89: return "ACPI On";
    case 109:return "ACPI Off";
    //unknown
    default:return "?";
    }
}

void show_information(PBYTE pBuffer, DWORD dwBytesRead)
{
    PBYTE pBufferEnd;
    PEVENTLOGRECORD pEventLog;

    ULONGLONG ullTimeStamp = 0;
    ULONGLONG SecsTo1970 = 116444736000000000ll;
    SYSTEMTIME st;
    FILETIME ftutc, ftLocal;

    DWORD event_code;
    BOOL bMatched;
    const char* event_source;

    pBufferEnd = pBuffer + dwBytesRead;

    while ((pBuffer < pBufferEnd) && (g_max_events > 0)) {
        pEventLog = (PEVENTLOGRECORD)pBuffer;

        event_source = (char*)pEventLog + sizeof(EVENTLOGRECORD);
        event_code = pEventLog->EventID & 0xFFFF;
        bMatched = FALSE;

        if (strcmp(event_source, "Microsoft-Windows-Kernel-Power") == 0) {
            bMatched = TRUE;
        } else if (strcmp(event_source, "Microsoft-Windows-Kernel-General") == 0) {
            if ((event_code == 12) || (event_code == 13)) {
                bMatched = TRUE;
            }
        }

        if (bMatched) {
            ullTimeStamp = (ULONGLONG)pEventLog->TimeGenerated * 10000000ll + SecsTo1970;
            ftutc.dwHighDateTime = (DWORD)((ullTimeStamp >> 32) & 0xFFFFFFFF);
            ftutc.dwLowDateTime  = (DWORD)(ullTimeStamp & 0xFFFFFFFF);
            FileTimeToLocalFileTime(&ftutc, &ftLocal);
            FileTimeToSystemTime(&ftLocal, &st);

            if ((st.wYear != g_prev_st.wYear) ||
                (st.wMonth != g_prev_st.wMonth) ||
                (st.wDay != g_prev_st.wDay))
            {
                if (g_max_events != MAX_EVENTS) {
                    printf("\n");
                }
                printf("=== %04d-%02d-%02d ===\n", st.wYear, st.wMonth, st.wDay);
            }

            printf("  %02d:%02d:%02d  ", st.wHour, st.wMinute, st.wSecond);
            printf("%5d (%s)", event_code, get_code_name(event_code));

            printf("\n");

            g_prev_st = st;
            g_max_events--;
        }

        pBuffer += pEventLog->Length;
    }
}


int main(int argc, char* argv[])
{
    HANDLE hEventLog = NULL;
    char pBuffer[0x8000];
    DWORD dwBytesToRead;
    DWORD dwBytesRead;
    DWORD dwMinimumBytesToRead;
    DWORD status;
    BOOL bok;

    if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0) {
            printf("%s [max_count]\n", argv[0]);
            return 0;
        }
        g_max_events = atoi(argv[1]);
    }
    
    hEventLog = OpenEventLog(NULL, "System");
    if (NULL == hEventLog) {
        printf("OpenEventLog failed with 0x%x.\n", GetLastError());
        return -1;
    }

    dwBytesToRead = sizeof(pBuffer);
    while (g_max_events > 0) {
        bok = ReadEventLog(hEventLog, 
            EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ,
            0, 
            pBuffer,
            dwBytesToRead,
            &dwBytesRead,
            &dwMinimumBytesToRead);

        if (!bok) {
            status = GetLastError();
            if (status == ERROR_INSUFFICIENT_BUFFER) {
                status = ERROR_SUCCESS;
                printf("ReadEventLog given buffer too small.\n");
                break;
            }

            if (status != ERROR_HANDLE_EOF) {
                printf("ReadEventLog failed with %lu.\n", status);
            }
            break;

        } else {
            show_information(pBuffer, dwBytesRead);
        }
    }

    if (hEventLog) {
        CloseEventLog(hEventLog);
    }


    return 0;
}

