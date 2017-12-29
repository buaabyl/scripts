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
#  2017.06.01      created by {user}
#
import sys
import os
import re
import getopt
import glob
import subprocess

if __name__ == '__main__':
    args = []
    if len(sys.argv) > 1:
        args.extend(sys.argv[1:])
    else:
        args.append('.')
    
    fpath = []
    for dpath in args:
        fpath.extend(glob.glob(dpath))

    allfiles = []
    ignores = []
    for startpath in fpath:
        l = startpath.split('\\')
        if '.git' in l:
            ignores.append(startpath)
            continue
        if os.path.isfile(startpath):
            allfiles.append(startpath)
            continue
        for prefix, dirs, files in os.walk(startpath):
            for fn in files:
                fullpath = os.path.join(prefix, fn)
                l = fullpath.split('\\')
                if '.git' in l:
                    ignores.append(fullpath)
                    continue
                allfiles.append(fullpath)

    MAX_ITEM = 100
    i = 0
    n = len(allfiles)
    while i < n:
        subfiles = allfiles[i:i+MAX_ITEM]
        cmd = ['touchall']
        cmd.extend(subfiles)
        res = subprocess.check_output(cmd)
        res = res.decode('GBK')
        sys.stdout.write(res)
        print()
        i = i + MAX_ITEM

    for fn in ignores:
        print('I', fn)


