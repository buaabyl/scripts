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
#include <string.h>
#include "libusbids.h"

int libusbids_parse_hex(const char* s, int* out)
{
    char c;
    int v = 0;
    int base = 10;

    if ((s == NULL) || (s[0] == '\0')) {
        return 0;
    }

    if ((s[0] == '0') && (s[1] == 'x')) {
        base = 16;
        s+=2;
    } else if ((s[0] == '0') && (s[1] == 'X')) {
        base = 16;
        s+=2;
    } else if (s[0] == '0') {
        base = 8;
        s++;
    } else {
        base = 10;
    }

    while ((c = *s++)) {
        if (('0' <= c) && (c <= '9')) {
            v = v * base + c - '0';
        } else if (('a' <= c) && (c <= 'f')) {
            v = v * base + c - 'a' + 10;
        } else if (('A' <= c) && (c <= 'F')) {
            v = v * base + c - 'A' + 10;
        } else {
            return 0;
        }
    }

    *out = v;

    return 1;
}

usbids_vid_list_t* libusbids_load(const char* dbname)
{
    FILE* fp;
    usbids_vid_list_t root;
    usbids_vid_list_t* vid_node;
    usbids_pid_list_t* pid_node;
    char buffer[1024];
    char hexstr[16] = "0x0000";
    char* s;
    char* tail;
    int lineno;
    int vid;
    int pid;
    int processing = 0;

    root.next = NULL;
    vid_node = &root;
    pid_node = NULL;

    if ((fp = fopen(dbname, "r")) == NULL) {
        return NULL;
    }

    lineno = 0;
    while (!feof(fp)) {
        lineno++;

        //fgets included '\n'
        memset(buffer, 0, sizeof(buffer));
        if (fgets(buffer, sizeof(buffer), fp) == NULL) {
            break;
        }

        //ignore comment
        if (buffer[0] == '#') {
            continue;
        }

        //check format "VID4 PID4"
        //                  ^
        if (!processing) {
            s = strchr(buffer, ' ');
            if (s && (s - buffer == 4)) {
                processing = 1;
            }
        }
        if (!processing) {
            continue;
        }

        //when the VID PID list end, 
        //there are must be an empty line.
        if (buffer[0] == '\n') {
            break;
        }

        //valid "VID4 PID4"
        s = strchr(buffer, ' ');
        if (s && (s - buffer == 4)) {
            //skip white space from prefix
            while (*++s == ' ') {
                if (*s == '\0') {
                    break;
                }
            }
            if (*s == '\0') {
                printf("Error: invalid format on line %d\n", lineno);
                break;
            }

            //skip '\n' at the end
            //skip white space from suffix
            tail = s + strlen(s) - 1;
            while (*--tail == ' ') {
                if (tail <= s) {
                    break;
                }
            }
            if (tail <= s) {
                printf("Error: invalid format on line %d\n", lineno);
                break;
            }
            tail[1] = '\0';

            memcpy(&hexstr[2], buffer, 4);
            libusbids_parse_hex(hexstr, &vid);

            vid_node->next = (usbids_vid_list_t*)malloc(sizeof(usbids_vid_list_t));
            vid_node = vid_node->next;
            vid_node->vid           = vid;
            vid_node->vendor_name   = strdup(s);
            vid_node->devices       = NULL;
            pid_node = NULL;

            continue;
        }

        //an element with previou VID
        s = buffer;
        if (s[0] == '\t') {
            s++;
        }
        if (s[0] == ' ') {
            while (*++s == ' ') {
            }
        }

        memcpy(&hexstr[2], s, 4);
        libusbids_parse_hex(hexstr, &pid);
        s += 4;

        //skip spaces and find
        while (*++s == ' ') {
            if (*s == '\0') {
                break;
            }
        }
        if (*s == '\0') {
            printf("Error: invalid format on line %d\n", lineno);
            break;
        }
        
        //remove spaces on tail
        tail = s + strlen(s) - 1;
        while (*--tail == ' ') {
            if (tail <= s) {
                break;
            }
        }
        if (tail <= s) {
            printf("Error: invalid format on line %d\n", lineno);
            break;
        }
        tail[1] = '\0';

        //append to list with previou VID
        if (pid_node == NULL) {
            pid_node = (usbids_pid_list_t*)malloc(sizeof(usbids_pid_list_t));
            vid_node->devices = pid_node;
        } else {
            pid_node->next = (usbids_pid_list_t*)malloc(sizeof(usbids_pid_list_t));
            pid_node = pid_node->next;
        }
        pid_node->pid         = pid;
        pid_node->device_name = strdup(s);
        pid_node->next        = NULL;
    }

    //printf("parsed lines %d\n", lineno);

    return root.next;
}

void libusbids_destory(usbids_vid_list_t* vids)
{
    usbids_vid_list_t* vid_node;
    usbids_pid_list_t* pid_node;

    while (vids) {
        vid_node = vids;
        vids = vids->next;

        while (vid_node->devices) {
            pid_node = vid_node->devices;
            vid_node->devices = vid_node->devices->next;

            if (pid_node->device_name) {
                free(pid_node->device_name);
            }
            free(pid_node);
        }

        if (vid_node->vendor_name) {
            free(vid_node->vendor_name);
        }
        free(vid_node);
    }
}

void libusbids_print(const usbids_vid_list_t* vids)
{
    const usbids_vid_list_t* vid_node;
    const usbids_pid_list_t* pid_node;

    for (vid_node = vids;vid_node != NULL;vid_node = vid_node->next) {
        printf(".VID 0x%04x \"%s\"\n", vid_node->vid, vid_node->vendor_name);
        for (pid_node = vid_node->devices;pid_node != NULL;pid_node = pid_node->next) {
            printf(" .PID 0x%04x \"%s\"\n", pid_node->pid, pid_node->device_name);
        }
    }
}

int libusbids_search(
        const usbids_vid_list_t* vids,
        int vid,
        int pid,
        usbids_info_t* result)
{
    const usbids_vid_list_t* vid_node;
    const usbids_pid_list_t* pid_node;

    for (vid_node = vids;vid_node != NULL;vid_node = vid_node->next) {
        if (vid_node->vid == vid) {
            break;
        }
    }
    if (vid_node == NULL) {
        return 0;
    }
    result->vid = vid;
    result->pid = pid;
    result->vendor_name = vid_node->vendor_name;
    result->device_name = NULL;

    for (pid_node = vid_node->devices;pid_node != NULL;pid_node = pid_node->next) {
        if (pid_node->pid == pid) {
            break;
        }
    }
    if (pid_node != NULL) {
        result->device_name = pid_node->device_name;
    }

    return 1;
}

#ifdef SELF_TEST
int main(int argc, char* argv[])
{
    int vid;
    int pid;
    usbids_vid_list_t* vids;
    usbids_info_t info;

    if (argc != 3) {
        printf("usage: %s vid pid\n", argv[0]);
        return -1;
    }

    if (!libusbids_parse_hex(argv[1], &vid)) {
        printf("Invalid VID\n");
        return -1;
    }

    if (!libusbids_parse_hex(argv[2], &pid)) {
        printf("Invalid PID\n");
        return -1;
    }

    printf("Searching VID 0x%04X, PID 0x%04X\n", vid, pid);
    vids = libusbids_load(USBIDS);
    if (vids) {
        //libusbids_print(vids);

        if (libusbids_search(vids, vid, pid, &info)) {
            printf("Vendor: \"%s\"\n", info.vendor_name);
            printf("Device: \"%s\"\n", info.device_name);
        }


        libusbids_destory(vids);
        vids = NULL;
    }

    return 0;
}
#endif

