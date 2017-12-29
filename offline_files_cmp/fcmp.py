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
# 2016.11.03    rewrite whole codes
#               compare sha1 when two db have sha1 values
# 2016.11.04    find file which moved
# 2017.03.09    not compare mtime directly, compare absolute of substrate value inside 1 second.
#

import os
import sys
import stat
import time
import json
import getopt
import platform
import hashlib
import itertools

VERSION = "2016.11.04"

def file_put_json(fn, h):
    s = json.dumps(h, indent=4, ensure_ascii=False)
    f = open(fn, 'w', encoding="UTF-8")
    f.write(s)
    f.close()

def file_get_json(fn):
    f = open(fn, 'r', encoding="UTF-8")
    s = f.read()
    f.close()
    return json.loads(s)

class ColorConsole(object):
    STD_INPUT_HANDLE  = -10
    STD_OUTPUT_HANDLE = -11
    STD_ERROR_HANDLE  = -12

    C_BLACK        = 0
    C_WHITE        = 15

    C_GREY         = 7
    C_DARKER_GREY  = 8

    C_TEAL         = 11
    C_DARKER_TEAL  = 3

    C_BLUE         = 9
    C_DARKER_BLUE  = 1

    C_GREEN        = 10
    C_DARKER_GREEN = 2

    C_PURPLE       = 5
    C_LIGHT_PURPLE = 13

    C_RED          = 12
    C_DARKER_RED   = 4

    C_YELLOW       = 14
    C_DARKER_YELLOW= 6


    def __init__(self):
        import ctypes
        self.dll = ctypes.windll.kernel32
        self.stdout_handle = self.dll.GetStdHandle(-11)

    def setColor(self, color):
        if self.stdout_handle != -1:
            self.dll.SetConsoleTextAttribute(self.stdout_handle, color)

    def reset(self):
        if self.stdout_handle != -1:
            self.dll.SetConsoleTextAttribute(self.stdout_handle, self.C_GREY)

def print_usage(progname):
    print('usage: %s file1 file2' % os.path.split(progname)[1])

def valid_filename(fn):
    fn = os.path.split(fn)[1]
    if fn.startswith('check') and fn.endswith('.json'):
        return False
    return True

def analysis_two_db(db1, db2):
    cmp_sha1 = False
    if 'sha1' in db1 and 'sha1' in db2:
        cmp_sha1 = True

    # convert list to map
    files1 = list(filter(lambda fdb:valid_filename(fdb['path']), db1['files']))
    files1 = dict((fdb['path'], fdb) for fdb in files1)

    files2 = list(filter(lambda fdb:valid_filename(fdb['path']), db2['files']))
    files2 = dict((fdb['path'], fdb) for fdb in files2)

    # course compare 
    set1 = set(files1)
    set2 = set(files2)

    files_common = set1 & set2

    only1   = set1 - set2
    m1names = dict([(fn, files1[fn]) for fn in only1])

    only2   = set2 - set1
    m2names = dict([(fn, files2[fn]) for fn in only2])

    del set1
    del set2

    # result database
    files_same          = []
    files_mtime_changed = []
    files_size_changed  = []
    files_sha1_changed  = []
    files_only1         = list(only1)
    files_only2         = list(only2)
    files_move          = []

    # compare file move
    if cmp_sha1:
        db1sha1 = {}
        db2sha1 = {}
        for fn, fdb in m1names.items():
            sha1val = fdb['sha1']
            if sha1val not in db1sha1:
                db1sha1[sha1val] = []
            db1sha1[sha1val].append(fn)
        for fn, fdb in m2names.items():
            sha1val = fdb['sha1']
            if sha1val not in db2sha1:
                db2sha1[sha1val] = []
            db2sha1[sha1val].append(fn)

        set1 = set(db1sha1)
        set2 = set(db2sha1)
        set_same  = set1 & set2
        set_only1 = set1 - set2
        set_only2 = set2 - set1

        l = [(sha1val, (db1sha1[sha1val], db2sha1[sha1val])) for sha1val in set_same]
        files_move = dict(l)

        l = [db1sha1[sha1val] for sha1val in set_only1]
        files_only1 = itertools.chain(*l)

        l = [db2sha1[sha1val] for sha1val in set_only2]
        files_only2 = itertools.chain(*l)

    # fine compare
    for fn in files_common:
        if files1[fn]['size'] != files2[fn]['size']:
            files_size_changed.append(fn)
            continue

        if cmp_sha1:
            if files1[fn]['sha1'] != files2[fn]['sha1']:
                files_sha1_changed.append(fn)
                continue

        difftime = int(files1[fn]['mtime']) - int(files2[fn]['mtime'])
        if difftime < -1 or difftime > 1:
            files_mtime_changed.append(fn)
            continue

        files_same.append(fn)

    result = {
        'same'  : files_same,
        'mtime' : files_mtime_changed,
        'size'  : files_size_changed,
        'sha1'  : files_sha1_changed,
        'only1' : files_only1,
        'only2' : files_only2,
        'move'  : files_move,
    }

    return result

def print_result(result):
    console_ctl = ColorConsole()
    print("Shortcut '=' equal, 'M' modify, 'T' touch")
    print('')

    #console_ctl.setColor(ColorConsole.C_DARKER_GREY)
    #n = len(result['same'])
    #l = sorted(result['same'])[:50]
    #for fn in l:
    #    print(' .. = .. %s' % fn)
    #n = n - len(l)
    #if n > 0:
    #    print(' .. = .. (display 50 max, another %d equal files) ......' % (n))

    console_ctl.setColor(ColorConsole.C_LIGHT_PURPLE)
    for fn in sorted(result['only1']):
        print('[<<<O   ] %s' % fn)

    console_ctl.setColor(ColorConsole.C_GREEN)
    for fn in sorted(result['only2']):
        print('[   O>>>] %s' % fn)

    console_ctl.setColor(ColorConsole.C_RED)
    for fn in sorted(result['size']):
        print('[   M   ] %s' % fn)
    for fn in sorted(result['sha1']):
        print('[   M   ] %s' % fn)

    console_ctl.setColor(ColorConsole.C_GREY)
    for fn in sorted(result['mtime']):
        print('[   T   ] %s' % fn)

    print('')

    if len(result['move']) <= 0:
        return

    for sha1val, (l1, l2) in result['move'].items():
        console_ctl.setColor(ColorConsole.C_WHITE)
        print('[  SHA  ] %s' % sha1val)
        console_ctl.setColor(ColorConsole.C_DARKER_GREY)
        for fn in l1:
            print('[<<<O   ] %s' % fn)
        for fn in l2:
            print('[   O>>>] %s' % fn)

    console_ctl.reset()


if __name__ == '__main__':
    print('Version:', VERSION)

    opts, args = getopt.getopt(sys.argv[1:], '', ['html'])
    if len(args) != 2:
        print_usage(sys.argv[0])
        sys.exit(-1)

    cmp_sha1 = False
    db1 = file_get_json(args[0])
    db2 = file_get_json(args[1])

    result = analysis_two_db(db1, db2)
    print_result(result)


