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
#
import sys
import os
import re
import getopt
import glob

INCLUDED_FILES = [
        '.h', '.c', '.hpp', '.cpp', '.cc',
        '.ac', '.am',
        '.v', '.vhd',
        '.md',
        '.py', '.pyx']

def count_lines(fn):
    f = open(fn, 'rb')
    d = f.read()
    d = d.decode('UTF-8', 'ignore')
    f.close()
    
    lines = d.split('\n')
    lineno = 0
    for line in lines:
        if len(line) == 0:
            continue
        if re.match(r'^\W+$', line):
            continue
        lineno = lineno + 1

    return lineno;

def scan_dirs(path, l):
    for prefix, dirs, files in os.walk(path):
        for fn in files:
            ext = os.path.splitext(fn)[1]
            ext = ext.lower()
            if ext not in INCLUDED_FILES:
                continue
            fn = os.path.join(prefix, fn)
            nr_lines = count_lines(fn)
            l.append((fn, nr_lines))

if __name__ == '__main__':
    l = []

    if len(sys.argv) == 1:
        scan_dirs('.', l)

    for path in sys.argv[1:]:
        if os.path.isfile(path):
            nr_lines = count_lines(path)
            l.append((path, nr_lines))
        else:
            scan_dirs(path, l)

    nr_total_lines = 0
    for fn, nr_lines in l:
        print('%7d %s' % (nr_lines, fn))
        nr_total_lines = nr_total_lines + nr_lines
    print('%7d total' % (nr_total_lines))


