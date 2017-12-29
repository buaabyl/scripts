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
import shutil
import operator

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
        print('usage: script.py result.json outdir')
        sys.exit(-1)

    db = load_json(sys.argv[1])
    prefix = sys.argv[2]
    for fn in db:
        if not os.path.exists(fn):
            continue

        dirname, filename = os.path.split(fn)
        outdir = os.path.join(prefix, dirname)
        outfilename = os.path.join(outdir, filename)
        if not os.path.exists(outdir):
            print('mkdir -p ' + outdir)
            os.makedirs(outdir)

        print('moving to ' + outfilename)
        shutil.move(fn, outfilename)





