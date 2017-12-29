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
import json

if __name__ == '__main__':
    if len(sys.argv) == 1:
        print('usage: script input.json output.json')
        sys.exit(-1)

    f = open(sys.argv[1], 'r', encoding='UTF-8')
    d = f.read()
    f.close()

    db = json.loads(d)
    jstr = json.dumps(db, indent=4, ensure_ascii=False)

    if len(sys.argv) == 2:
        print(jstr)
    else:
        print('wrote "%s"' % sys.argv[2])
        f = open(sys.argv[2], 'w', encoding='UTF-8')
        f.write(jstr)
        f.close()

