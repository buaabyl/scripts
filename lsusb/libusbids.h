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

#ifndef LIBUSBIDS_H_BF5193C0_0517_11E6_BCE6_005056C00008_INCLUDED_
#define LIBUSBIDS_H_BF5193C0_0517_11E6_BCE6_005056C00008_INCLUDED_

#define USBIDS  "usb.ids"

typedef struct _pid_list_t{
    int                 pid;
    char*               device_name;
    struct _pid_list_t* next;
}usbids_pid_list_t;

typedef struct _vid_list_t{
    int                 vid;
    char*               vendor_name;
    usbids_pid_list_t*  devices;
    struct _vid_list_t* next;
}usbids_vid_list_t;

typedef struct {
    int                 vid;
    const char*         vendor_name;
    int                 pid;
    const char*         device_name;
}usbids_info_t;


int libusbids_parse_hex(const char* s, int* out);

usbids_vid_list_t* libusbids_load(const char* dbname);
void libusbids_destory(usbids_vid_list_t* vids);

void libusbids_print(const usbids_vid_list_t* vids);

int libusbids_search(
        const usbids_vid_list_t* vids,
        int vid,
        int pid,
        usbids_info_t* result);

#endif

