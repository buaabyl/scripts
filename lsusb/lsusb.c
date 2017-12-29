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

#include <libusb-1.0/libusb.h>
#include "libusbids.h"
#include "usbids_embbed.h"

#define PID_1E26 "Intel(R) 7 Series/C216 Chipset Family USB Enhanced Host Controller"
#define PID_1E2D "Intel(R) 7 Series/C216 Chipset Family USB Enhanced Host Controller"
#define PID_1E31 "Intel(R) USB 3.0 eXtensible Host Controller"

static int using_embbed = 0;

void print_extra(libusb_device* dev, struct libusb_device_descriptor* desc)
{
    int ret;
    char buf[1000];
    int langid = 0x0409;//USA
    int config;
    libusb_device_handle* handle;
    int i;

    ret = libusb_open(dev, &handle);
    if (ret < 0) {
        return;
    }

    ret = libusb_get_configuration(handle, &config);
    if (ret < 0) {
        printf("Error: libusb_get_configuration return %d\n", ret);
    } else {
        printf("  Config: %d\n", config);
    }

    if (desc->iManufacturer > 0) {
        memset(buf, 0, sizeof(buf));
        libusb_get_string_descriptor_ascii(handle, desc->iManufacturer, buf, sizeof(buf));
        printf("  Manufacturer(%d): \"%s\"\n", desc->iManufacturer, buf);
    }

    if (desc->iProduct > 0) {
        memset(buf, 0, sizeof(buf));
        libusb_get_string_descriptor_ascii(handle, desc->iProduct, buf, sizeof(buf));
        printf("  Product(%d): \"%s\"\n", desc->iProduct, buf);
    }

    if (desc->iSerialNumber > 0) {
        memset(buf, 0, sizeof(buf));
        //buffer:
        //  number of bytes (included self)
        //  ?
        //  lo(unicode_char[0])
        //  hi(unicode_char[0])
        //  lo(unicode_char[1])
        //  hi(unicode_char[1])
        //  ...
        //  lo(unicode_char[n-1])
        //  hi(unicode_char[n-1])
        //
        //  not NULL terminal!
        //
        //ret = libusb_get_string_descriptor(handle, 
        //        desc->iSerialNumber, langid, buf, sizeof(buf));
        //for (i = 2;i < ret;i+=2) {
        //    printf(" %02x, %c\n", buf[i] & 0xFF, buf[i] & 0xFF);
        //}

        libusb_get_string_descriptor_ascii(handle, desc->iSerialNumber, buf, sizeof(buf));
        printf("  SerialNumber(%d): \"%s\"\n", desc->iSerialNumber, buf);
    }

    libusb_close(handle);
}

typedef struct {
    libusb_device* device;
    int address;
    char ports[10];
}_device_t;

int print_location(libusb_device* device)
{
    int i;
    int ret;
    struct libusb_device_descriptor desc;
	uint8_t loc[8]; 

    ret = libusb_get_device_descriptor(device, &desc);
    if (ret < 0) {
        printf("Error: libusb_get_device_descriptor return %d\n", ret);
        return 0;
    }

    printf(">> bus %d", libusb_get_bus_number(device));
    ret = libusb_get_port_numbers(device, loc, sizeof(loc));
    if (ret) {
        printf(", ports ");
        for (i = 0;i < ret;i++) {
            printf("%d", loc[i]);
            if (i != ret - 1) {
                printf(".");
            }
        }
    } else {
        printf(", root");
    }
    printf("\n");

    return 1;
}

int main(int argc, char* argv[])
{
    usbids_vid_list_t* usbids;
    usbids_vid_list_t* plist;
    usbids_pid_list_t* pnode;
    usbids_info_t info;
    int ret;
    int i;
    int j;
    int k;
    int idx_interface;
    int nr_devices;
	libusb_device **devices;
    libusb_device* device;
    _device_t* devices_sorted;
    _device_t device_tmp;

    struct libusb_device_descriptor desc;
	struct libusb_config_descriptor* config;
	const struct libusb_interface* interface;
	const struct libusb_interface_descriptor* altsetting;
	const struct libusb_endpoint_descriptor* endpoint;
	uint8_t loc[8]; 

    usbids = libusbids_load(USBIDS);
    if (usbids != NULL) {
        plist = usbids;
        while (plist->next != NULL) {
            if (plist->vid == 0x8086) {
                break;
            }
            plist = plist->next;
        }
        if ((plist != NULL) && (plist->devices != NULL)) {
            pnode = plist->devices;
            while (pnode->next != NULL) {
                pnode = pnode->next;
            }

            pnode->next = (usbids_pid_list_t*)malloc(sizeof(usbids_pid_list_t));
            pnode = pnode->next;
            pnode->pid          = 0x1E26;
            pnode->device_name  = strdup(PID_1E26);
            pnode->next         = NULL;

            pnode->next = (usbids_pid_list_t*)malloc(sizeof(usbids_pid_list_t));
            pnode = pnode->next;
            pnode->pid          = 0x1E2D;
            pnode->device_name  = strdup(PID_1E2D);
            pnode->next         = NULL;

            pnode->next = (usbids_pid_list_t*)malloc(sizeof(usbids_pid_list_t));
            pnode = pnode->next;
            pnode->pid          = 0x1E31;
            pnode->device_name  = strdup(PID_1E31);
            pnode->next         = NULL;
        }

    } else {
        using_embbed = 1;
        printf("Info: fail to load \"usb.ids\", using embbed db\n");
    }

    ret = libusb_init(NULL);
    if (ret < 0) {
        printf("Error: libusb_init return %d\n", ret);
        libusbids_destory(usbids);
        return -1;
    }

	nr_devices = libusb_get_device_list(NULL, &devices);
    if (nr_devices > 0) {
        devices_sorted = (_device_t*)malloc(sizeof(_device_t) * nr_devices);
        memset(devices_sorted, 0, sizeof(_device_t) * nr_devices);

        //cache instances
        for (i = 0;i < nr_devices;i++) {
            if (devices[i] == NULL) {
                continue;
            }

            devices_sorted[i].device   = devices[i];
            devices_sorted[i].address  = libusb_get_device_address(devices[i]);
            devices_sorted[i].ports[0] = libusb_get_bus_number(devices[i]);
            ret = libusb_get_port_numbers(devices[i], loc, sizeof(loc));
            if (ret) {
                for (j = 0;j < ret;j++) {
                    devices_sorted[i].ports[j+1] = loc[j];
                }
            }
        }


        //sort
        for (i = 0;i < nr_devices;i++) {
            for (j = i+1;j < nr_devices;j++) {
                if (strcmp(devices_sorted[i].ports, devices_sorted[j].ports) > 0) {
                    device_tmp = devices_sorted[i];
                    devices_sorted[i] = devices_sorted[j];
                    devices_sorted[j] = device_tmp;
                }
            }
        }

        //print
        printf("> Summary\n");
        for (i = 0;i < nr_devices;i++) {
            device = devices_sorted[i].device;
            if (device == NULL) {
                continue;
            }
            if (!print_location(device)) {
                break;
            }
        }
        printf("\n");

        printf("> Detials\n");
        for (i = 0;i < nr_devices;i++) {
            device = devices_sorted[i].device;
            if (device == NULL) {
                continue;
            }
            if (!print_location(device)) {
                break;
            }

            ret = libusb_get_device_descriptor(device, &desc);
            if (ret < 0) {
                printf("Error: libusb_get_device_descriptor return %d\n", ret);
                return 0;
            }

            printf(">> VID_%04X&PID_%04X ", desc.idVendor, desc.idProduct);
            printf("(");
            if (desc.bcdUSB == 0x0300) {
                printf("USB3.0");
            } else if (desc.bcdUSB == 0x0210) {
                printf("USB3.0(on 2.0 port)");
            } else if (desc.bcdUSB == 0x0200) {
                printf("USB2.0");
            } else if (desc.bcdUSB == 0x0110) {
                printf("USB1.1");
            } else if (desc.bcdUSB == 0x0100) {
                printf("USB1.0");
            } else if (desc.bcdUSB == 0x0000) {
                printf("Host Controler");
            } else {
                printf("bcdUSB = 0x%04x", desc.bcdUSB);
            }
            printf(")\n");

            if (using_embbed) {
                memset(&info, 0, sizeof(info));
                libusbids_search_embbed(desc.idVendor, desc.idProduct, &info);
            } else {
                libusbids_search(usbids, desc.idVendor, desc.idProduct, &info);
            }
            if (info.vendor_name != NULL) {
                printf("  Vendor       : %s\n", info.vendor_name);
                printf("  Device       : %s\n", info.device_name);
            }

            printf("  Address      : %d\n", libusb_get_device_address(device));

            printf("  Class        : ");
            switch (desc.bDeviceClass) {
                case LIBUSB_CLASS_PER_INTERFACE:
                    printf("interface");break;
                case LIBUSB_CLASS_AUDIO:
                    printf("audio");break;
                case LIBUSB_CLASS_COMM:
                    printf("comm");break;
                case LIBUSB_CLASS_HID:
                    printf("hid");break;
                case LIBUSB_CLASS_PHYSICAL:
                    printf("physical");break;
                case LIBUSB_CLASS_PRINTER:
                    printf("printer");break;
                case LIBUSB_CLASS_IMAGE:
                    printf("image");break;
                case LIBUSB_CLASS_MASS_STORAGE:
                    printf("mass");break;
                case LIBUSB_CLASS_HUB:
                    printf("hub");break;
                case LIBUSB_CLASS_DATA:
                    printf("data");break;
                case LIBUSB_CLASS_SMART_CARD:
                    printf("smartcard");break;
                case LIBUSB_CLASS_CONTENT_SECURITY:
                    printf("security");break;
                case LIBUSB_CLASS_VIDEO:
                    printf("video");break;
                case LIBUSB_CLASS_PERSONAL_HEALTHCARE:
                    printf("healthcare");break;
                case LIBUSB_CLASS_DIAGNOSTIC_DEVICE:
                    printf("diagnostic");break;
                case LIBUSB_CLASS_WIRELESS:
                    printf("wireless");break;
                case LIBUSB_CLASS_APPLICATION:
                    printf("app");break;
                case LIBUSB_CLASS_VENDOR_SPEC:
                    printf("spec");break;
                default:
                    printf("etc.");break;
            }
            printf("\n");


            if (desc.bcdUSB == 0x0000) {
                printf("\n");
                continue;
            }

            if (desc.bcdUSB > 0) {
                print_extra(device, &desc);
            }

            ret = libusb_get_config_descriptor_by_value(device, 1, &config);
            if ((ret == LIBUSB_SUCCESS) && (config->interface != NULL)) {
                interface = config->interface;

                for (idx_interface = 0;
                    idx_interface < config->bNumInterfaces;
                    idx_interface++)
                {
                    altsetting = interface[idx_interface].altsetting;

                    for (j = 0;j < interface[idx_interface].num_altsetting;j++) {
                        printf("  Interface%d\n", j);

                        for (k = 0;k < altsetting[j].bNumEndpoints;k++) {
                            endpoint = altsetting[j].endpoint;
                            printf("     ");
                            if (endpoint[k].bEndpointAddress & LIBUSB_ENDPOINT_IN) {
                                printf("IN,  ");
                            } else {
                                printf("OUT, ");
                            }
                            printf("0x%02x, %d bytes\n",
                                    endpoint[k].bEndpointAddress,
                                    endpoint[k].wMaxPacketSize);
                        }
                    }
                }
                libusb_free_config_descriptor(config);
            }

            printf("\n");
        }

        //free
        free(devices_sorted);
        devices_sorted = NULL;

        //If the unref_devices parameter is set,
        //the reference count of each device in the list is decremented by 1.
        libusb_free_device_list(devices, 1/*unref*/);
    }

    if (!using_embbed) {
        libusbids_destory(usbids);
    }

	libusb_exit(NULL);
    return 0;
}

