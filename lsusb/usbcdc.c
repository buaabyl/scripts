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

//  Peripheral USB2.0
//  Class comm
//  VID 04B4 "Cypress Semiconductor Corp."
//  PID 4718 "(null)"
//  Config: 1
//  Manufacturer(1): "Cypress"
//  Product(2): "EZ-USB FX2 VIRTUAL COM PORT"
//  SerialNumber(3): "Cypress CY7C68013A"
//  .NumInterfaces = 2
//    .NumEndpoints 1
//     .IN,  0x81, 16 bytes
//    .NumEndpoints 2
//     .IN,  0x88, 512 bytes
//     .OUT, 0x01, 64 bytes
//
#define EP_IN    0x88
#define EP_OUT   0x01

#define ACM_CTRL_DTR   0x01
#define ACM_CTRL_RTS   0x02

libusb_device_handle* usbcdc_open(libusb_context* ctx)
{
    int ret;
    // 9600 8N1, 9600 = 0x2580 ~> 0x80, 0x25 in little endian
    unsigned char encoding[] = {0x80, 0x25, 0x00, 0x00, 0x00, 0x00, 0x08};
    char buffer[100];
    libusb_device_handle* dev;

    dev = libusb_open_device_with_vid_pid(ctx, 0x04B4, 0x4718);
    if (dev == NULL) {
        printf("Error: libusb_open_device_with_vid_pid fail\n");
        return NULL;
    }

    printf("Information:\n");
    libusb_get_string_descriptor_ascii(dev, 1, buffer, sizeof(buffer));
    printf("  Vendor  \"%s\"\n", buffer);
    libusb_get_string_descriptor_ascii(dev, 2, buffer, sizeof(buffer));
    printf("  Product \"%s\"\n", buffer);
    libusb_get_string_descriptor_ascii(dev, 3, buffer, sizeof(buffer));
    printf("  Serial  \"%s\"\n", buffer);
    printf("\n");

    ret = libusb_set_configuration(dev, 1/*configuration*/);
    if (ret != LIBUSB_SUCCESS) {
        printf("Error: libusb_set_configuration return %d\n", ret);
        goto L_CONFIG_ERROR;
    }

    ret = libusb_claim_interface(dev, 0/*interface control*/);
    if (ret != LIBUSB_SUCCESS) {
        printf("Error: libusb_claim_interface(0) return %d\n", ret);
        goto L_INTERFACE_ERROR0;
    }

    ret = libusb_claim_interface(dev, 1/*interface stream*/);
    if (ret != LIBUSB_SUCCESS) {
        printf("Error: libusb_claim_interface(1) return %d\n", ret);
        goto L_INTERFACE_ERROR1;
    }

    // - set line state
    ret = libusb_control_transfer(dev,
            0x21/*request_type*/, 0x22/*bRequest*/, 
            ACM_CTRL_DTR | ACM_CTRL_RTS /*value*/, 0/*index*/,
            NULL, 0, 0);
    if (ret < 0) {
        printf("Error: libusb_control_transfer(line) return %d\n", ret);
        goto L_CONTROL_ERROR;
    }

    // - set line encoding
    ret = libusb_control_transfer(dev,
            0x21/*request_type*/, 0x20/*bRequest*/,
            0/*value*/, 0/*index*/,
            encoding, sizeof(encoding), 0);
    if (ret < 0) {
        printf("Error: libusb_control_transfer(encoding) return %d\n", ret);
        goto L_CONTROL_ERROR;
    }

    printf("Success: initialize done\n");

    return dev;

L_CONTROL_ERROR:
    libusb_release_interface(dev, 1);
L_INTERFACE_ERROR1:
    libusb_release_interface(dev, 0);
L_INTERFACE_ERROR0:
L_CONFIG_ERROR:
L_DEV_ERROR:
    libusb_close(dev);

    return NULL;
}

int usbcdc_close(libusb_device_handle* dev)
{
    int ret;

    ret = libusb_release_interface(dev, 0);
    if (ret != LIBUSB_SUCCESS) {
        printf("Error: libusb_release_interface(0) return %d\n", ret);
    }

    ret = libusb_release_interface(dev, 1);
    if (ret != LIBUSB_SUCCESS) {
        printf("Error: libusb_release_interface(1) return %d\n", ret);
    }

    libusb_close(dev);

    return 0;
}

int usbcdc_send(libusb_device_handle* dev, unsigned char* buf, int len)
{
    int ret;
    int sent = 0;

    ret = libusb_bulk_transfer(dev, EP_OUT, buf, len, &sent, 1000);
    if (ret != LIBUSB_SUCCESS) {
        printf("Error: libusb_bulk_transfer(send) return %d\n", ret);
        return -1;
    }

    return sent;
}

int usbcdc_recv(libusb_device_handle* dev, unsigned char* buf, int len)
{
    int ret;
    int received = 0;

    ret = libusb_bulk_transfer(dev, EP_IN, buf, len, &received, 1000);
    if (ret != LIBUSB_SUCCESS) {
        printf("Error: libusb_bulk_transfer(recv) return %d\n", ret);
        return -1;
    }

    return received;
}

void print_raw(char* buffer, int bufsize)
{
    int i;

    printf("\"");
    for (i = 0;i < bufsize;i++) {
        switch (buffer[i]) {
            case '\r':printf("\\r");break;
            case '\n':printf("\\n");break;
            case '\b':printf("\\b");break;
            default:printf("%c", buffer[i]);break;
        }
    }
    printf("\"\n");
}

int main(int argc, char* argv[])
{
    int ret;
    libusb_context* ctx;
    libusb_device_handle* dev = NULL;
    int nr_devices;
    char buffer[100];
    int sent;
    int received;
    int i;

    ret = libusb_init(&ctx);
    if (ret < 0) {
        printf("Error: libusb_init return %d\n", ret);
        return -1;
    }

    //libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_DEBUG);
    
    dev = usbcdc_open(ctx);
    if (dev != NULL) {
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "0\r\n");
        printf("TX: ");
        print_raw(buffer, strlen(buffer));
        sent = usbcdc_send(dev, buffer, strlen(buffer));
        Sleep(100);

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "1\r\n");
        printf("TX: ");
        print_raw(buffer, strlen(buffer));
        sent = usbcdc_send(dev, buffer, strlen(buffer));
        Sleep(100);

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "0\r\n");
        printf("TX: ");
        print_raw(buffer, strlen(buffer));
        sent = usbcdc_send(dev, buffer, strlen(buffer));
        Sleep(100);

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "reload\r\n");
        printf("TX: ");
        print_raw(buffer, strlen(buffer));
        sent = usbcdc_send(dev, buffer, strlen(buffer));

        memset(buffer, 0, sizeof(buffer));
        received = usbcdc_recv(dev, buffer, sizeof(buffer));
        printf("RX: ");
        print_raw(buffer, received);

        usbcdc_close(dev);

    } else {
        printf("Error: libusb_open_device_with_vid_pid fail\n");
    }

	libusb_exit(ctx);

    return 0;
}

