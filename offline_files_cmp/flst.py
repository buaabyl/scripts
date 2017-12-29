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
#               reused exist hash result when not force
#               add file detect, when modify recalculate sha1
#               add force sha1 calculate
#               support continue hash
# 2016.11.04    show how many files found before calculate
# 2016.11.07    'sha1' is default mode.
#               check filename before calculate sha1
# 2016.11.09    fixed divied by zero
# 2017.03.09    not compare mtime directly, compare absolute of substrate value inside 1 second.
# 2017.09.04    change default name `check-...` to `zcheck-...`
#

import os
import sys
import stat
import time
import json
import glob
import ctypes
import getopt
import hashlib
import platform
import itertools

VERSION   = "2016.11.07"
BLOCKSIZE = 0x40000 #256KB

# auto save result when processed 1GB files.
# normal process speed is 100MB/s, 
# so it equ to save every 10 seconds.
AUTO_SAVE_TH = 1024 * 1024 * 1024

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

def console_set_title(s):
    ctypes.windll.kernel32.SetConsoleTitleW(s)

def sha1sum(fn, startup_path, filesize, base_bytes, total_bytes):
    realpath = os.path.join(startup_path, fn)
    f = open(realpath, 'rb')

    short_fn = fn
    if len(short_fn) > 60:
        short_fn = fn[:20] + ' ...... ' + fn[-30:]

    digest = hashlib.sha1()
    if not filesize:
        st = os.stat(fn)
        filesize = st[stat.ST_SIZE]

    n = filesize // BLOCKSIZE

    if filesize < BLOCKSIZE:
        d = f.read()
        digest.update(d)
    else:
        i = 1
        d = f.read(BLOCKSIZE)
        processed_bytes = base_bytes

        while d:
            processed_bytes = processed_bytes + len(d)
            percent = processed_bytes * 100.0 / total_bytes
            msg = '%6.2f%%, %6.2f%%, %s\r' % (percent, i * 100.0 / n, short_fn)
            sys.stdout.write(msg)

            digest.update(d)
            d = f.read(BLOCKSIZE)
            i = i + 1

    f.close()

    hexdigest = digest.hexdigest()
    sys.stdout.write(' %s *%s\n' % (hexdigest, short_fn))

    return hexdigest

def load_exists_dbs(cfg):
    if not cfg['sha1']:
        return 

    pattern = os.path.join(cfg['path'], 'zcheck-*.json')

    # finding exists dbs, and reverse sort by mtime
    # using newest db instead of older one.
    l = glob.glob(pattern)
    l = [os.path.realpath(v) for v in l]
    l = [(v, os.stat(v)[stat.ST_MTIME]) for v in l]
    l.sort(reverse=True, key=lambda pairs:pairs[1])

    # load db and filter valid one
    l = [file_get_json(pairs[0]) for pairs in l]
    l = list(filter(lambda subdb:'sha1' in subdb, l))
    l = list(filter(lambda subdb:'files' in subdb, l))
    l = [subdb['files'] for subdb in l]

    # merge multi files-db list into one list
    l = list(itertools.chain(*l))
    l = [(fdb['path'], fdb) for fdb in l]

    # convert to dict
    db = {}
    for name, fdb in l:
        if name in db:
            continue
        db[name] = fdb

    return db

def create_files_db(cfg):
    lst_dirs  = []
    lst_files = []

    scan_path       = cfg['path']
    output_filename = cfg['output']

    for prefix, dirs, files in os.walk(scan_path):
        lst_dirs.extend([os.path.join(prefix, entry) for entry in dirs])
        lst_files.extend([os.path.join(prefix, entry) for entry in files])

    pattern = os.path.join(cfg['path'], 'zcheck*.json')
    l = glob.glob(pattern)
    m = dict([(os.path.realpath(v), True) for v in l])
    m[output_filename] = True

    lst_dirs = [os.path.realpath(entry) for entry in lst_dirs]
    lst_files= [os.path.realpath(entry) for entry in lst_files]
    lst_files= list(filter(lambda entry:entry not in m, lst_files))

    dirs_db  = []
    files_db = []
    db = {'files':files_db, 'dirs':dirs_db}

    n_skips_chars = len(scan_path) + 1
    for entry in lst_dirs:
        st = os.stat(entry)
        dirs_db.append({
            'type'  : 'dir', 
            'path'  : entry[n_skips_chars:],
            'size'  : 0,
            'ctime' : st[stat.ST_CTIME],
            'mtime' : st[stat.ST_MTIME]
        })

    for entry in lst_files:
        st = os.stat(entry)
        files_db.append({
            'type'  : 'file', 
            'path'  : entry[n_skips_chars:],
            'size'  : st[stat.ST_SIZE],
            'ctime' : st[stat.ST_CTIME],
            'mtime' : st[stat.ST_MTIME],
        })

    return db

def pretty_time(duration):
    t_sec = int(duration)
    t_min = t_sec // 60
    t_sec = t_sec - t_min * 60
    t_hour= t_min // 60
    t_min = t_min - t_hour * 60
    if t_hour > 0:
        return '%d hours, %d minutes, %d seconds' % (t_hour, t_min, t_sec)
    elif t_min > 0:
        return '%d minutes, %d seconds' % (t_min, t_sec)
    elif t_sec > 0:
        return '%d seconds' % t_sec
    else:
        return 'soon'

def pretty_size(nr_bytes):
    nr_kbytes = nr_bytes / 1024
    nr_mbytes = nr_kbytes / 1024
    nr_gbytes = nr_mbytes / 1024

    if nr_gbytes > 1.0:
        return '%.3f GB' % nr_gbytes
    if nr_mbytes > 1.0:
        return '%.3f MB' % nr_mbytes
    if nr_kbytes > 1.0:
        return '%.3f KB' % nr_kbytes
    return '%d Bytes' % nr_bytes


def print_summary(duration, total_bytes):
    t_sec = int(duration)
    t_min = t_sec // 60
    t_sec = t_sec - t_min * 60
    t_hour= t_min // 60
    t_min = t_min - t_hour * 60
    if t_hour > 0:
        print('> sha1sum: %d hours, %d minutes, %d seconds' % (t_hour, t_min, t_sec))
    elif t_min > 0:
        print('> sha1sum: %d minutes, %d seconds' % (t_min, t_sec))
    elif t_sec > 0:
        print('> sha1sum: %d seconds' % t_sec)

    if duration == 0:
        speed = 0
    else:
        speed = total_bytes / duration
    speed_kbps = speed / 1024
    speed_mbps = speed_kbps / 1024

    if speed_mbps > 0:
        print('> sha1sum: %.3f MB/s' % speed_mbps)
    elif speed_kbps > 0:
        print('> sha1sum: %.3f KB/s' % speed_kbps)
    elif speed > 0:
        print('> sha1sum: %.3f B/s' % speed)


def update_files_hash(cfg, db):
    if not cfg['sha1']:
        return 

    t_begin = time.time()

    scan_path   = cfg['path']
    oldhash     = cfg['old']
    sha1db      = {}
    db['sha1']  = sha1db

    # check filename
    for fdb in db['files']:
        try:
            fdb['path'].encode('GBK')
        except:
            print('Error filename:')
            print('', repr(fdb['path']))
            print('')
            raise

    # check whether need calculate sha1 or just reuse
    if cfg['force']:
        l_to_process = db['files']
    else:
        l_to_process = []
        for fdb in db['files']:
            do_sha1 = False
            filename = fdb['path']
            if filename in oldhash:
                oldfdb = oldhash[filename]
                difftime = fdb['mtime'] - oldfdb['mtime']

                if fdb['size'] != oldfdb['size']:
                    do_sha1 = True
                elif difftime < -1 or difftime > 1:
                    do_sha1 = True
                elif 'sha1' not in oldfdb:
                    do_sha1 = True
            else:
                do_sha1 = True

            if do_sha1:
                l_to_process.append(fdb)
                continue

            hexdigest = oldfdb['sha1']

            fdb['sha1'] = hexdigest
            if hexdigest not in sha1db:
                sha1db[hexdigest] = []
            sha1db[hexdigest].append(fdb['path'])

    # save temporary db
    dump_db_to_file(cfg, db)

    # calculate sha1sum
    nr_total        = len(db['files'])
    processed_bytes = 0
    total_bytes     = sum([fdb['size'] for fdb in l_to_process])
    saved_bytes     = 0

    # just calculate as 100MB/s
    left_time = total_bytes // (100 * 1024 * 1024)

    print('> info: begin hash %d files, %s' % (len(l_to_process), pretty_size(total_bytes)))
    console_set_title('*** Info*** "%.60s" *** %s left ***' % (cfg['path'], pretty_time(left_time)))

    # calculate sha1
    for fdb in l_to_process:
        hexdigest = sha1sum(fdb['path'], 
                startup_path    = scan_path,
                filesize        = fdb['size'], 
                base_bytes      = processed_bytes, 
                total_bytes     = total_bytes)

        fdb['sha1'] = hexdigest
        if hexdigest not in sha1db:
            sha1db[hexdigest] = []
        sha1db[hexdigest].append(fdb['path'])

        processed_bytes = processed_bytes + fdb['size']

        # save every 100MB
        if processed_bytes - saved_bytes > AUTO_SAVE_TH:
            dump_db_to_file(cfg, db)
            saved_bytes = processed_bytes

        # update title
        if processed_bytes == 0:
            continue
        t_end     = time.time()
        duration  = t_end - t_begin
        left_time = duration * (total_bytes - processed_bytes) / processed_bytes
        console_set_title('*** Info*** "%.60s" *** %s left ***' % (cfg['path'], pretty_time(left_time)))

    # print information
    t_end = time.time()
    duration = t_end - t_begin
    print_summary(duration, total_bytes)

    nr_process = len(l_to_process)
    print('> sha1sum: %d reused, %d calculated' % (nr_total - nr_process, nr_process))

    dump_db_to_file(cfg, db)

def dump_db_to_file(cfg, db):
    file_put_json(cfg['output'], db)

def parse_commandline(argv):
    cfg = {
        'sha1'   : True,
        'force'  : False,
        'old'    : {},
        'output' : time.strftime("zcheck-%Y.%m.%d-%H%M%S.json", time.localtime())
    }

    opts, args = getopt.getopt(argv, 'cfo:', ['sha1', 'force', 'fast'])
    if len(args) <= 0:
        raise BaseException()

    for k, v in opts:
        if k == '--sha1' or k == '-c':
            cfg['sha1'] = True
        elif k == '--fast':
            cfg['sha1'] = False
        elif k == '-f' or k == '--force':
            cfg['force'] = True
        elif k == '-o' or k == '--output':
            cfg['output'] = v

    cfg['path'] = os.path.realpath(args[0])
    cfg['path'] = os.path.split(os.path.join(cfg['path'], 'dummy'))[0]

    if len(args) == 2:
        cfg['output'] = args[1]
    cfg['output'] = os.path.realpath(cfg['output'])
    if os.path.splitext(cfg['output'])[1].lower() != '.json':
        cfg['output'] = cfg['output'] + '.json'

    return cfg

def print_usage(progname):
    print('usage: %s [options] path [output_db]' % os.path.split(progname)[1])
    print('options:')
    print('  %2s  %-22s %s' % ('-o', '--output', 'force output db name, default is time-besed name'))
    print('  %2s  %-22s %s' % ('-f', '--force', 'force calculate all sha1 when --sha1 enable'))
    print('  %2s  %-22s %s' % ('-c', '--sha1', 'calculate sha1 hash'))
    print('  %2s  %-22s %s' % ('',   '--fast', 'disable sha1 generate, just size and mtime'))

if __name__ == '__main__':
    print('Version: ', VERSION)

    try:
        cfg = parse_commandline(sys.argv[1:])
    except BaseException:
        print_usage(sys.argv[0])
        sys.exit(-1)
    except:
        print_usage(sys.argv[0])
        raise

    if not os.path.isdir(cfg['path']):
        print('"%s" not exists' % cfg['path'])
        sys.exit(-1)

    print('> output "%s"' % cfg['output'])

    # loading exist database
    olddb = load_exists_dbs(cfg)
    if olddb:
        cfg['old'] = olddb
        print('> info: load %d sha1sum value from exists db' % len(olddb))
    else:
        print('> info: nothing found from exists db')

    # generate files list
    db = create_files_db(cfg)
    dump_db_to_file(cfg, db)

    total_bytes = 0
    for fdb in db['files']:
        total_bytes = total_bytes + fdb['size']
    print('> info: found %d files, %s' % (len(db['files']), pretty_size(total_bytes)))

    # calculate sha1sum
    update_files_hash(cfg, db)

    # read back and check
    db = file_get_json(cfg['output'])
    print('> info: number of dirs ', len(db['dirs']))
    print('> info: number of files %d, %s' % (len(db['files']), pretty_size(total_bytes)))
    print('> info: wrote %d bytes to \"%s\"' % (os.stat(cfg['output'])[stat.ST_SIZE], cfg['output']))


