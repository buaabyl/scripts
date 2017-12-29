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
# 2016.11.08   created by buaa.byl@gmail.com
# 2016.11.09   show error message when fail
#
import sys
import os
import stat
import json
import getopt
import subprocess

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

def print_usage(prog):
    print('usage: %s [options] db.json' % os.path.split(prog)[1])
    print('  this script walk through db.json and update all files timestamp')
    print('  when "--set" option valid, update files timestamp from db.json')
    print('')
    print('options:')
    print('  %2s  %-22s %s' % ('-f', '--force', 'force continue even error occupy'))
    print('  %2s  %-22s %s' % ('',   '--set',   'set file time from db.json'))

if __name__ == '__main__':
    do_set_time = False
    force = False
    opts, args = getopt.getopt(sys.argv[1:], 'f', ['set', 'force'])
    for k, v in opts:
        if k == '--set':
            do_set_time = True
        elif k == '-f' or k == '--force':
            force = True

    print('load "%s"' % args[0])
    db = file_get_json(args[0])

    if do_set_time:
        nr_files = 0
        for fn, ts in db['time'].items():
            try:
                subprocess.check_output(['cftime', fn, *ts], universal_newlines=True)
            except subprocess.CalledProcessError as e:
                print('[error]   %s' % fn)
                print(e.output)
                if not force:
                    break
            nr_files = nr_files + 1

        print('updated %d files' % nr_files)

    else:
        rawtime = {}
        for fdb in db['files']:
            try:
                res = subprocess.check_output(['cftime', fdb['path']], universal_newlines=True)
            except subprocess.CalledProcessError as e:
                print('[error]   %s' % fn)
                print(e.output)
                if not force:
                    break

            record = res.split('\n')[0].split(':')[1].split(' ')
            record = list(filter(lambda v:len(v) > 0, record))
            rawtime[fdb['path']] = record

        db['time'] = rawtime

        print('updated "%s"' % args[0])
        file_put_json(args[0], db)


