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
 *  2016.10.11  first release
 *  2017.01.01  fixed wireless adapter not access
 */

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <iphlpapi.h>

// defines:
//
// mingw64/x86_64-w64-mingw32/include/ipifcons.h
//

#include "manuf.h"
#include "if_type.h"

#define VERSION "2017.01.01"

//MAC Address database
//  https://code.wireshark.org/review/gitweb?p=wireshark.git;a=blob_plain;f=manuf
//  https://code.wireshark.org/review/gitweb?p=wireshark.git;a=blob;f=manuf.tmpl;h=8758cf4bcf5e4752fca4ec5eeb7958d0eb06bfe5;hb=HEAD
//
//Adapter list in registers:
//  "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Class\{4D36E972-E325-11CE-BFC1-08002BE10318}"
//  subkey "0014" is Tap-Win32
//      "DriverDesc"        = "TAP-Windows Adapter V9"
//      "NetCfgInstanceId"  = "{73C5CE10-BF93-4E58-95B9-5D24CFBA413C}"
//
//Connection list in registers:
//  "HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\Network\{4D36E972-E325-11CE-BFC1-08002BE10318}"
//  subkey "{73C5CE10-BF93-4E58-95B9-5D24CFBA413C}\Connection"
//      "Name"  = "tap001"  (which the name display in "Control Panel\Network and Internet\Network Connections")
//
//Some commandline:
//  netsh interface ip show config
//  netsh interface ip show addresses
//
//  netsh interface ip set address [name=]InterfaceName \
//      [source=]{ \
//          dhcp | \
//          static [addr=]IPAddress[mask=]SubnetMask \
//              [gateway=]{none | DefaultGateway [[gwmetric=]GatewayMetric]} \
//      }
//
//  netsh interface ip set address name="Local Area Connection" static 192.168.0.100 255.255.255.0 192.168.0.1 1
//  netsh interface ip set address "Local Area Connection" dhcp
//  netsh interface ip set dns "Local Area Connection" static 192.168.0.200
//
char* get_connection_name(const char* guid)
{
    const char prefix[] = "SYSTEM\\"
        "ControlSet001\\Control\\Network\\"
        "{4D36E972-E325-11CE-BFC1-08002BE10318}";
    char keypath[4096];
    HKEY h;
    DWORD ret;
    DWORD type;
    char* s;
    DWORD len;

    sprintf(keypath, "%s\\%s\\Connection", prefix, guid);
    ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keypath, 0, KEY_READ, &h);
    if (ret != ERROR_SUCCESS) {
        return NULL;
    }

    len = 0;
    ret = RegGetValue(h, NULL, "Name", RRF_RT_REG_SZ, &type, NULL, &len);
    if (ret != ERROR_SUCCESS) {
        goto L_ERROR_0;
    }

    s = (char*)malloc(len + 1);
    ret = RegGetValue(h, NULL, "Name", RRF_RT_REG_SZ,
            &type, s, &len);
    if (ret != ERROR_SUCCESS) {
        goto L_ERROR_1;
    }

    RegCloseKey(h);
    return s;

L_ERROR_1:
    free(s);

L_ERROR_0:
    RegCloseKey(h);
    return NULL;
}

const manuf_t* find_vendor(unsigned char mac[6])
{
    int i;
    int n;
    int mask;
    int bytes;
    int bits;
    unsigned char bmask;

    n = sizeof(wireshark_macids)/sizeof(wireshark_macids[0]);

    for (i = 0;i < n;i++) {
        mask = wireshark_macids[i].mask;
        bytes = mask / 8;
        bits  = mask % 8;

        if (memcmp(mac, wireshark_macids[i].mac, bytes) != 0) {
            continue;
        }
        if (bytes == 6) {
            return &wireshark_macids[i];
        }

        bmask = 0xFFu << (8-bits);
        if ((mac[bytes] & bmask) == wireshark_macids[i].mac[bytes]) {
            return &wireshark_macids[i];
        }
    }

    return NULL;
}

int main(int argc, char* argv[])
{
    PIP_ADAPTER_INFO ipAdapterInfo;
    PIP_ADAPTER_INFO pnic;
    PIP_ADDR_STRING pips;
    DWORD dwSize;
    DWORD ret;
    DWORD nrAdapters;
    const manuf_t* info;
    char* name;
    int i;

    printf("Version: " VERSION "\n");
    printf("Build  : " __TIME__ "\n");
    printf("\n");

    dwSize = 0;
    ret = GetAdaptersInfo(NULL, &dwSize);
    if (ret == ERROR_BUFFER_OVERFLOW) {
        ipAdapterInfo = (PIP_ADAPTER_INFO)malloc(dwSize);
        memset(ipAdapterInfo, 0, dwSize);
        ret = GetAdaptersInfo(ipAdapterInfo, &dwSize);
    }

    if (ret != ERROR_SUCCESS) {
        printf("GetAdaptersInfo, GetLastError=%d\n", GetLastError());
        return -1;
    }

    nrAdapters = 0;
    for (pnic = ipAdapterInfo;pnic != NULL;pnic = pnic->Next) {
        name = get_connection_name(pnic->AdapterName);
        if (name != NULL) {
            printf(">>%s (%s)\n", pnic->AdapterName, name);
            free(name);
        } else {
            printf(">>%s\n", pnic->AdapterName);
        }

        printf("  Description: \"%s\"\n", pnic->Description);

        if ((pnic->Type == IF_TYPE_ETHERNET_CSMACD) ||
            (pnic->Type == IF_TYPE_IEEE80211))
        {
            printf("  MAC        : ");

            for (i = 0;i < pnic->AddressLength;i++) {
                printf("%.2X", pnic->Address[i]);
                if (i != pnic->AddressLength-1) {
                    printf(":");
                }
            }

            info = find_vendor(pnic->Address);
            if (info == NULL) {
                printf("\n");
            } else {
                if (strlen(info->desc) > 0) {
                    printf(" (%s: %s)\n", info->vendor, info->desc);
                } else {
                    printf(" (%s)\n", info->vendor);
                }
            }

            if (pnic->DhcpEnabled) {
                printf("  DHCP Server: ");
                if ((pnic->DhcpServer.IpAddress.String == NULL) ||
                    (pnic->DhcpServer.IpAddress.String[0] == '\0'))
                {
                    printf("0.0.0.0");
                }
                for (pips = &pnic->DhcpServer;pips != NULL;pips = pips->Next) {
                    printf("%s ", pips->IpAddress.String);
                }
                printf("\n");

            } else {
                printf("  DHCP       : disabled\n");
            }

            for (pips = &pnic->IpAddressList;pips != NULL;pips = pips->Next) {
                printf("  IP/MASK    : %s/%s\n", pips->IpAddress.String, pips->IpMask.String);
            }
            printf("  GW         : ");
            for (pips = &pnic->GatewayList;pips != NULL;pips = pips->Next) {
                printf("%s ", pips->IpAddress.String);
            }
            printf("\n");

        } else {
            printf("  Type = %d \"%s\"\n", pnic->Type, get_if_type_name(pnic->Type));
        }

        printf("\n");

        nrAdapters++;
    }


    free(ipAdapterInfo);

    return 0;
}

