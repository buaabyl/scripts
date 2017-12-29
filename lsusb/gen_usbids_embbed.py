#!/usr/bin/env python3
# -*- coding:utf-8 -*-
#
#                   GNU GENERAL PUBLIC LICENSE
#                       Version 2, June 1991
#  
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
#
# 2016.11.11   created by buaa.byl@gmail.com
#
import sys
import re
import os
import json
import uuid
import hashlib
from mako.template import Template

h_tpl = '''\
#ifndef ${header_macro}
#define ${header_macro}

int libusbids_search_embbed(
        int vid,
        int pid,
        usbids_info_t* result);

#endif

'''


c_tpl = '''\
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libusbids.h"
#include "${header_name}"

typedef struct {
    int vid;
    int pid;
    const char* vendor;
    const char* device;
}usbids_vid_pid_t;

static const usbids_vid_pid_t _ids[] = {
%for record in db:
    {${record['vid_hex']}, ${record['pid_hex']}, "${record['vendor']}", "${record['device']}"},
%endfor
};

int libusbids_search_embbed(
        int vid,
        int pid,
        usbids_info_t* result)
{
    int nr_records;
    int i;

    nr_records = sizeof(_ids)/sizeof(_ids[0]);

    for (i = 0;i < nr_records;i++) {
        if (_ids[i].vid == vid) {
            result->vid = vid;
            result->pid = pid;
            result->vendor_name = _ids[i].vendor;
            break;
        }
    }
    if (i == nr_records) {
        return 0;
    }

    for (;i < nr_records;i++) {
        if (_ids[i].vid != vid) {
            break;
        }
        if (_ids[i].pid == pid) {
            result->vid = vid;
            result->pid = pid;
            result->vendor_name = _ids[i].vendor;
            result->device_name = _ids[i].device;
            break;
        }
    }

    return 1;
}

'''

def generate_uuid(fn):
    filepath = os.path.split(fn)[1]
    filename, fileext = os.path.splitext(filepath)

    #such as '57f9f97a-9a5a-11e6-85c9-000000000000'
    #last part is mac address, may private issuse!
    uuids    = str(uuid.uuid1()).split('-')
    uuids[4] = hashlib.sha1(uuids[4].encode('UTF-8')).hexdigest()[:12]
    uuidstr  = '-'.join(uuids).upper()

    filename_upp = filename.upper()
    fileext_upp  = fileext.upper()
    headfilemacro = '%s%s_%s_INCLUDED_' % (filename_upp, fileext_upp, uuidstr)
    headfilemacro = headfilemacro.replace('.','_')
    headfilemacro = headfilemacro.replace('-','_')
    return headfilemacro


def file_get_contents(fn):
    f = open(fn, 'rb')
    d = f.read()
    d = d.decode('UTF-8', 'replace')
    d = d.replace('\r\n', '\n')
    f.close()
    return d

def file_put_contents(fn, d):
    f = open(fn, 'w', encoding='UTF-8')
    f.write(d)
    f.close()

def file_put_json(fn, d):
    f = open(fn, 'w', encoding='UTF-8')
    jstr = json.dumps(d, ensure_ascii=False, indent=4)
    f.write(jstr)
    f.close()

def escape(s):
    s = s.replace('\\', '\\\\')
    s = s.replace('?', '')
    s = s.replace('"', '\\"')
    return s

if __name__ == '__main__':
    text = file_get_contents(sys.argv[1])
    lines = text.split('\n')
    lineno = 0
    n = len(lines)
    print(n)

    pattern_vid = re.compile(r'^([0-9a-fA-F]{4})\s+(.+)$')
    pattern_pid = re.compile(r'^\s+([0-9a-fA-F]{4})\s+(.+)$')

    while lineno < n:
        line = lines[lineno]
        if len(line) == 0:
            pass
        elif line[0] == '#':
            pass
        elif pattern_vid.match(line):
            break
        lineno = lineno + 1

    if lineno >= n:
        print('Invalid db')
        sys.exit(-1)

    vid    = -1
    vendor = ''
    l = []
    x86_vid = 0x8086
    x86_vendor = ''
    while lineno < n:
        line = lines[lineno]
        if len(line) == 0:
            break
        lineno = lineno + 1

        res = pattern_vid.match(line)
        if res:
            vid = int(res.groups()[0], 16)
            vendor = res.groups()[1]
            continue

        if vid == -1:
            continue

        if vid == x86_vid:
            x86_vendor = vendor

        res = pattern_pid.match(line)
        if res:
            pid = int(res.groups()[0], 16)
            device = res.groups()[1]
            l.append({
                'vid': vid,
                'pid': pid,
                'vid_hex': '0x%04x' % vid, 
                'pid_hex': '0x%04x' % pid, 
                'vendor': escape(vendor),
                'device': escape(device)
            })


    x86_devices = [
        (0x1E26, "Intel(R) 7 Series/C216 Chipset Family USB Enhanced Host Controller"),
        (0x1E2D, "Intel(R) 7 Series/C216 Chipset Family USB Enhanced Host Controller"),
        (0x1E31, "Intel(R) USB 3.0 eXtensible Host Controller")
    ]
    for pid, device in x86_devices:
        l.append({
            'vid': x86_vid,
            'pid': pid,
            'vid_hex': '0x%04x' % x86_vid, 
            'pid_hex': '0x%04x' % pid, 
            'vendor': escape(x86_vendor),
            'device': escape(device)
        })

    l.sort(key=lambda v:v['vid'] * 0x10000 + v['pid'])

    fn = 'usbids_embbed.h'
    h = Template(h_tpl)
    header_macro = generate_uuid(fn)
    res = h.render_unicode(header_macro=header_macro)
    print('wrote %s' % fn)
    file_put_contents(fn, res)

    hdr_fn = fn
    fn = os.path.splitext(hdr_fn)[0] + '.c'
    h = Template(c_tpl)
    res = h.render_unicode(header_name=hdr_fn, db=l)
    print('wrote %s' % fn)
    file_put_contents(fn, res)


