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
//link Advapi32.lib


//  typedef struct _EVENTLOGRECORD {
//      DWORD Length;
//      DWORD Reserved;
//      DWORD RecordNumber;
//      DWORD TimeGenerated;
//      DWORD TimeWritten;
//      DWORD EventID;
//      WORD  EventType;
//      WORD  NumStrings;
//      WORD  EventCategory;
//      WORD  ReservedFlags;
//      DWORD ClosingRecordNumber;
//      DWORD StringOffset;
//      DWORD UserSidLength;
//      DWORD UserSidOffset;
//      DWORD DataLength;
//      DWORD DataOffset;
//  } EVENTLOGRECORD, *PEVENTLOGRECORD;
//
//
//EventID::
//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +---+-+-+-----------------------+-------------------------------+
// |Sev|C|R|     Facility          |               Code            |
// +---+-+-+-----------------------+-------------------------------+
//
//Sev
//  Severity. The severity is defined as follows:
//  00 - Success
//  01 - Informational
//  10 - Warning
//  11 - Error
//
//C
//  Customer bit. This bit is defined as follows:
//  0 - System code
//  1 - Customer code
//
//R
//  Reserved bit.
//
//Facility
//  Facility code. This value can be FACILITY_NULL.
//
//Code
//  Status code for the facility.
//
const char* K_EVENT_TYPE_NAMES[] = {
    "Error", "Warning", "Informational", "Audit Success", "Audit Failure"
};

DWORD DumpRecordsInBuffer(PBYTE pBuffer, DWORD dwBytesRead)
{
    PBYTE pBufferEnd;
    PEVENTLOGRECORD pEventLog;
    DWORD event_type;

    ULONGLONG ullTimeStamp = 0;
    ULONGLONG SecsTo1970 = 116444736000000000ll;
    SYSTEMTIME st;
    FILETIME ftutc, ftLocal;

    const char* event_source;


    pBufferEnd = pBuffer + dwBytesRead;

    while (pBuffer < pBufferEnd) {
        pEventLog = (PEVENTLOGRECORD)pBuffer;

        //time
        ullTimeStamp = (ULONGLONG)pEventLog->TimeGenerated * 10000000ll + SecsTo1970;
        ftutc.dwHighDateTime = (DWORD)((ullTimeStamp >> 32) & 0xFFFFFFFF);
        ftutc.dwLowDateTime  = (DWORD)(ullTimeStamp & 0xFFFFFFFF);
        
        FileTimeToLocalFileTime(&ftutc, &ftLocal);
        FileTimeToSystemTime(&ftLocal, &st);
        printf("%04d-%02d-%02d %02d:%02d:%02d  ",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        //event type
        switch (pEventLog->EventType) {
            case EVENTLOG_ERROR_TYPE:
                event_type = 0;
                break;
            case EVENTLOG_WARNING_TYPE:
                event_type = 1;
                break;
            case EVENTLOG_INFORMATION_TYPE:
                event_type = 2;
                break;
            case EVENTLOG_AUDIT_SUCCESS:
                event_type = 3;
                break;
            case EVENTLOG_AUDIT_FAILURE:
                event_type = 4;
                break;
        }

        printf("type=%15s, ", K_EVENT_TYPE_NAMES[event_type]);
        printf("category=%5d, ", pEventLog->EventCategory);
        printf("code=%5d, ", pEventLog->EventID & 0xFFFF);

        event_source = (char*)pEventLog + sizeof(EVENTLOGRECORD);
        printf("source=%s\n", event_source);

        pBuffer += pEventLog->Length;
    }

    return 0;
}


int main(int argc, char* argv[])
{
    HANDLE hEventLog = NULL;
    char pBuffer[0x8000];
    int max_events = 2;
    DWORD dwBytesToRead;
    DWORD dwBytesRead;
    DWORD dwMinimumBytesToRead;
    DWORD status;
    BOOL bok;
    
    
    hEventLog = OpenEventLog(NULL, "System");
    if (NULL == hEventLog) {
        printf("OpenEventLog failed with 0x%x.\n", GetLastError());
        return -1;
    }

    dwBytesToRead = sizeof(pBuffer);
    while (max_events--) {
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
            DumpRecordsInBuffer(pBuffer, dwBytesRead);
        }
    }

    if (hEventLog) {
        CloseEventLog(hEventLog);
    }


    return 0;
}

