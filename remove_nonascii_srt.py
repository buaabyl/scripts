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

if __name__ == '__main__':
    f = open(sys.argv[1], 'r', encoding='UTF-8')
    d = f.read()
    f.close()

    fn = 'remove_nonascii.log'
    print('log file', fn)
    f = open(fn, 'w', encoding='UTF-8')

    l = []
    nr_ignore = 0
    begin = False
    for line in d.split('\n'):
        if len(line) == 0:
            l.append(line)

        if line == '41':
            begin = True

        if not begin:
            l.append(line)
            continue

        nr_no_ascii = 0
        for ch in line:
            if ord(ch) >= 0x80:
                nr_no_ascii = nr_no_ascii + 1
        if nr_no_ascii > len(line)/2:
            nr_ignore = nr_ignore + 1
            f.write('ignore %s\n' % line)
            continue

        l.append(line)


    f.close()


    fn = os.path.splitext(sys.argv[1])[0] + '.enu.new.srt'
    f = open(fn, 'w', encoding='UTF-8')
    f.write('\n'.join(l))
    f.close()
    print('wrote', fn)
        



