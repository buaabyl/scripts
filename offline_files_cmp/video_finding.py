#!/bin/python
# -*- coding:utf-8 -*-
#  
#  Copyright 2013 buaa.byl@gmail.com
#
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

import os
import re
import sys
import json
import operator

l_video_exts = ['.amr', '.mpg', '.m4v', '.avi', '.mov', '.mp4', '.3gp']
m_video_exts = {}
for k in l_video_exts:
    m_video_exts[k] = k

def dump_json(fn, h):
    s = json.dumps(h, indent=4)
    f = open(fn, 'w')
    f.write(s)
    f.close()

def load_json(fn):
    f = open(fn, 'r')
    s = f.read()
    f.close()
    return json.loads(s)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('usage: script.py db.json subdir')
        sys.exit(-1)


    db = load_json(sys.argv[1])
    subdir = sys.argv[2]

    files = db['files']
    map_ext = {}
    map_size= {}

    lst_moving = []

    for fdb in files:
        fn = fdb['path']
        if not fn.startswith(subdir):
            continue

        ext = os.path.splitext(fn)[1]
        if not ext or ext == '':
            print('Warning: ' + fn)
        ext = ext.lower()

        if ext in m_video_exts:
            lst_moving.append(fn)

        if ext not in map_ext:
            map_ext[ext] = 1
            map_size[ext] = fdb['size']
        else:
            map_ext[ext] = map_ext[ext] + 1
            map_size[ext] = map_size[ext] + fdb['size']
    
    pairs = sorted(map_size.items(), key=operator.itemgetter(1))
    for ext, size in pairs:
        count = map_ext[ext]
        size_kb = size/1024.0
        size_mb = size_kb/1024.0
        size_gb = size_mb/1024.0

        if size_gb > 1.0:
            print('%10s, %5d files, %10.3f GB' % (ext, count, size_gb))
        elif size_mb > 1.0:
            print('%10s, %5d files, %10.3f MB' % (ext, count, size_mb))
        elif size_kb > 1.0:
            print('%10s, %5d files, %10.3f KB' % (ext, count, size_kb))
        else:
            print('%10s, %5d files, %10.3f Bytes' % (ext, count, size))

    fn = re.sub('[^\w]', '_', subdir)
    fn = fn + '.result.json'
    print('dump moving list: ' + fn)
    dump_json(fn, lst_moving)


