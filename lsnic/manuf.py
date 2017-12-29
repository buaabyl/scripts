#!/usr/bin/env python
# -*- coding:utf-8 -*-
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2, or (at your option)
#  any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; see the file COPYING.  If not, write to
#  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
#
from __future__ import print_function

import sys
import os
import re

header = '''\
#ifndef WIRESHARK_MANUF_H
#define WIRESHARK_MANUF_H

typedef struct {
    int         mask;
    const char* mac;
    const char* vendor;
    const char* desc;
}manuf_t;

extern const manuf_t wireshark_macids[%d];

#endif

'''

c_begin = '''\
#include "manuf.h"

const manuf_t wireshark_macids[%d] = {
'''

c_end = '''\
};

'''


def parse_manuf(d):
    l = []

    lines = d.split('\n')
    for line in lines:
        if len(line) == 0:
            continue

        line = line.replace('\t', ' ')

        # skip white spaces
        if re.match(r'^[ ]+$', line):
            continue

        # skip empty comment
        if re.match(r'^[ ]*#', line):
            continue

        # extract mac math pattern
        idx = line.find(' ')
        mac = line[:idx]
        etc = line[idx+1:]

        mask = 48
        idx = mac.find('/')
        if idx > 0:
            mask = int(mac[idx+1:])
            mac  = mac[:idx]

        if ':' in mac:
            mac = mac.split(':')
        else:
            mac = mac.split('-')

        n = len(mac) * 8
        if n < mask:
            mask = n

        n = len(mac)
        mac.extend(['00'] * (6 - n))

        # extract vendor name and description
        idx = etc.find('#')
        desc = ''
        if idx > 0:
            desc = etc[idx+1:]
            desc = re.sub(r'^[ \t]+', '', desc)
            desc = re.sub(r'[ \t]+$', '', desc)
            desc = desc.replace('"', '\'')
            desc = desc.replace('\\', ' ')

            etc = etc[:idx]

        vendor = re.sub(r'^[ \t]+', '', etc)
        vendor = re.sub(r'[ \t]+$', '', vendor)

        l.append((mac, mask, vendor, desc))

    return l

if __name__ == '__main__':
    f = open('manuf')
    d = f.read()
    f.close()

    l = parse_manuf(d)

    print('wrote "manuf.h"')
    f = open('manuf.h', 'w')
    f.write(header % len(l))
    f.close()

    print('wrote "manuf.c"')
    f = open('manuf.c', 'w')
    f.write(c_begin % len(l))

    for mac, mask, vendor, desc in l:
        f.write('    {%2d, \"\\x' % mask)
        f.write('\\x'.join(mac))
        f.write('\", %-10s, \"%s\"},' % ('"' + vendor + '"', desc))
        f.write('\n')

    f.write(c_end)


