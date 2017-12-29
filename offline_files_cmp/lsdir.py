#!/usr/bin/env python3
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
# 2016.10.24    update python35
#
from __future__ import print_function 

import os
import re
import sys
import stat
import time
import platform
import json
import getopt
import subprocess
from mako.template import Template

'''
find.exe . -type d -mindepth 1 -maxdepth 1 -exec python lsdir.py \"{}\" ;
'''

VERSION = 'lsdir-2016.10.24'
SUFFIX  = '.lsdir.html'

# RAR5: 7z.exe v16.00+ or UnRAR.exe v5.00+
P7ZIP   = None
UNRAR   = None


template = u'''\
<!Doctype html>
<html>
<head>
<meta http-equiv=Content-Type content="text/html;charset=utf-8">
<title>${title}</title>
<style type="text/css">
    a:link      { text-decoration:none;}
    a:active    { text-decoration:blink}
    a:hover     { text-decoration:underline;} 
    a:visited   { text-decoration:none;}

    .toc_item {
        width: 100%;
        border-left: 1px solid red;
        margin-top: 5px;
    }
    .toc_item a {
        width: auto;
        padding: 10px;
        display: block;
        color: #000000;
        background-color: #EEEEEE;
    }
    .toc_item a:hover {
        color: #FFFFFF;
        background-color: #888888;
    }

    h2 {
        border-left: 1px dotted grey;
        border-bottom: 1px dotted grey;
        padding: 5px;
    }


    table, th, tr, td{
        border: 1px solid #000000;
        border-collapse: collapse;
    }
    th {
        color: white;
        background-color: #444444;
    }
    td {
        padding: 5px;
    }
    .text_right {
        text-align: right;
    }

    table, tr {
        width: 100%;
    }
    .time_td {
        width: 20%;
    }
    .ext_td {
        width: 5%;
    }
    .size_td {
    }
    .path_td {
    }

    /*for par2 and other equal files*/
    .lowlight {
        color: #AAAAAA;
    }

    .size_text {
        display:block;
        float: left;
        width: 50%;
    }
    .gbunit {
        display:block;
        float: left;
        width: 50%;
        color: #CC0000;
    }
    .mbunit {
        display:block;
        float: left;
        width: 50%;
        color: #33CC33;
    }
    .kbunit {
        display:block;
        float: left;
        width: 50%;
        color: #6666CC;
    }
    .bunit {
        display:block;
        float: left;
        width: 50%;
        color: #888888;
    }

    .row_even {
        background-color: #DDDDDD;
    }
    .row_odd {
        background-color: #EEEEEE;
    }
</style>
</head>

<body>

<h1>${title}</h1>


<div>Generator: ${version}</div>

<h2>TOC</h2>
<div class="toc_item"><a href="#files_by_name">Files (by name)</a></div>
<div class="toc_item"><a href="#files_by_size">Files (by size)</a></div>

% if len(db['dirs']) > 0:
<div class="toc_item"><a href="#dirs">Dirs</a></li></div>
% endif

<h3>Archives</h3>
% for info in archives:
<div class="toc_item"><a href="#files_in_${info[0]}">&nbsp;+&nbsp;${info[1]}</a></div>
% endfor 


<br />

Total size: ${size}

<h2><a name="files_by_name">Files (by name)</a></h2>
<table>
    <tr>
        <th>MTime</th>
        <th>Type</th>
        <th>Size</th>
        <th style="color:red">Path ↑</th>
    </tr>
% for f in db['files']:
    <tr class="${f['class']}">
        <td class="time_td">${f['mtime_str']}</td>
        <td class="ext_td">${f['ext']}</td>
        <td class="size_td"><div class="text_right">${f['pretty_size']}</div></td>
        <td class="path_td">${f['path']}</td>
    </tr>
% endfor
</table>

<h2><a name="files_by_size">Files (by size)</a></h2>
<table>
    <tr>
        <th>MTime</th>
        <th>Type</th>
        <th style="color:red">Size ↓</th>
        <th>Path</th>
    </tr>
% for f in db['files-by-size']:
    <tr class="${f['class']}">
        <td class="time_td">${f['mtime_str']}</td>
        <td class="ext_td">${f['ext']}</td>
        <td class="size_td"><div class="text_right">${f['pretty_size']}</div></td>
        <td class="path_td">${f['path']}</td>
    </tr>
% endfor
</table>

<h2><a name="dirs">Dirs</a></h2>
<table>
    <tr>
        <th>CTime</th>
        <th>Type</th>
        <th>Path</th>
    </tr>
% for d in db['dirs']:
    <tr class="${d['class']}">
        <td class="time_td">${f['ctime_str']}</td>
        <td class="ext_td">dir</td>
        <td>${d['path']}</td>
    </tr>
% endfor
</table>


<h2>Archives</h2>
% for info in archives:
<h3><a name="files_in_${info[0]}">${info[1]}</a></h2>
<pre>
${info[2]}
</pre>
% endfor

% for i in range(20):
<br />
% endfor

</body>
</html>
'''


if platform.system() == 'Windows':
    DELIMITER = '\\'
elif platform.system() == 'Linux':
    DELIMITER = '/'

def get_abspath(fn):
    '''
    generate dirs and files list
    with relative path.

    return {
        'dirs': [
            {'path':filepath, 'size':0, ... },
            ...
        ],
        'files': [
            {'path':filepath, 'size':100, ... },
            ...
        ],
    }
            
    '''
    absfn = os.path.abspath(fn)
    if not os.path.isfile(absfn):
        env = os.getenv('PATH')
        l = re.split(r'[ ]*;[ ]*', env)
        for v in l:
            absfn = os.path.join(v, fn)
            if os.path.isfile(absfn):
                return absfn
    return absfn

def gen_fileslist(path_to_scan):
    map_ignores = {"pagefile.sys":True}
    lst_dirs  = []
    lst_files = []

    #prepare path name
    if path_to_scan[-1] != DELIMITER:
        path_to_scan = path_to_scan + DELIMITER

    #generate top level dirs and files, skip ignore entry.
    for entry in os.listdir(path_to_scan):
        realpath = os.path.realpath(os.path.join(path_to_scan, entry))
        if entry in map_ignores:
            continue
        if os.path.isdir(realpath):
            lst_dirs.append(entry)
        else:
            lst_files.append(entry)

    #walk through sub directory
    lst_subdirs = []

    #make sure not included path_to_scan,
    #just generate relative path.
    len_prefix = len(path_to_scan)

    for subdir in lst_dirs:
        fullsubdir = os.path.join(path_to_scan, subdir)
        for root, dirs, files in os.walk(fullsubdir):
            for dirname in dirs:
                lst_subdirs.append(os.path.join(root, dirname)[len_prefix:])
            for filename in files:
                lst_files.append(os.path.join(root, filename)[len_prefix:])
    lst_dirs.extend(lst_subdirs)

    #fill dirs and files to result list
    lst_dirs_result = []
    lst_files_result = []

    for dirname in lst_dirs:
        dirstat = os.stat(os.path.join(path_to_scan, dirname))
        element = {
                'type':'dir', 
                'path':dirname,
                'size':0,
                'ctime':dirstat[stat.ST_CTIME],
                'mtime':dirstat[stat.ST_MTIME],
                }
        lst_dirs_result.append(element)

    for filename in lst_files:
        filestat = os.stat(os.path.join(path_to_scan, filename))
        element = {
                'type':'file', 
                'path':filename,
                'size':filestat[stat.ST_SIZE],
                'ext':os.path.splitext(filename)[1].lower(),
                'ctime':filestat[stat.ST_CTIME],
                'mtime':filestat[stat.ST_MTIME],
                }
        lst_files_result.append(element)

    return {'dirs':lst_dirs_result, 'files':lst_files_result}

def get_volume_label(absdir):
    res = re.match(r'^([a-zA-Z]):[\\]*$', absdir)
    if not res:
        return None

    drive = res.groups()[0]
    try:
        import win32api
        t = win32api.GetVolumeInformation("%s:\\" % drive)
        title = t[0]
    except:
        info = subprocess.check_output(['cmd', '/c', 'vol', '%s:' % drive])
        title = info.split('\n')[0][len(' Volume in drive R is'):]

    return title

def find_p7zip():
    global P7ZIP
    if not P7ZIP:
        for exename in ['7z.exe', '7za.exe']:
            fn = get_abspath(exename)
            if os.path.isfile(fn):
                P7ZIP = fn
                #print('found : "%s"' % P7ZIP)
                return True

    return False

def find_unrar():
    global UNRAR
    if not UNRAR:
        for exename in ['UnRAR.exe', 'RAR.exe']:
            fn = get_abspath(exename)
            if os.path.isfile(fn):
                UNRAR = fn
                #print('found : "%s"' % UNRAR)
                return True

    return False

def get_archive_files_list(lst_files, path_to_scan):
    global P7ZIP
    global UNRAR

    oldpwd = os.getcwd()
    os.chdir(path_to_scan)

    extlst = ['.zip', '.rar']
    if P7ZIP:
        extlst.append('.7z')
        if P7ZIP.endswith('7z.exe'):
            extlst.append('.iso')

    l = []
    i = 0
    try:
        for fdb in lst_files:
            fn = fdb['path']
            if fdb['ext'] not in extlst:
                continue
            if not os.path.isfile(fn):
                continue

            print(' - scan: "%s"' % fn);
            if fdb['ext'] == '.rar':
                #res = subprocess.check_output([UNRAR, 'la', '%s' % fn.encode('GBK')])
                res = subprocess.check_output([UNRAR, 'la', fn])
            else:
                res = subprocess.check_output([P7ZIP, 'l', fn])
            l.append((i, fn, res.decode('GBK')))
            i = i + 1

    except subprocess.CalledProcessError as e:
        print(e)
        raise

    finally:
        os.chdir(oldpwd)

    return l

def pretty_size_html(v):
    size_in_kb = v / 1024.0
    size_in_mb = size_in_kb / 1024.0
    size_in_gb = size_in_mb / 1024.0
    if size_in_gb > 1.0:
        return '<span class="size_text">%.3f</span><span class="gbunit">GB</span>' % size_in_gb
    if size_in_mb > 1.0:
        return '<span class="size_text">%.3f</span><span class="mbunit">MB</span>' % size_in_mb
    if size_in_kb > 1.0:
        return '<span class="size_text">%.3f</span><span class="kbunit">KB</span>' % size_in_kb
    return '<span class="size_text">%.3f</span><span class="bunit">Bytes</span>' % v

def pretty_size(v):
    size_in_kb = v / 1024.0
    size_in_mb = size_in_kb / 1024.0
    size_in_gb = size_in_mb / 1024.0
    if size_in_gb > 1.0:
        return '%.3f GBytes' % size_in_gb
    if size_in_mb > 1.0:
        return '%.3f MBytes' % size_in_mb
    if size_in_kb > 1.0:
        return '%.3f KBytes' % size_in_kb
    return '%.3f Bytes' % v

def pretty_db(l):
    total_size = 0
    i = 0
    for fdb in l['files']:
        fdb['pretty_size'] = pretty_size_html(fdb['size'])
        fdb['ctime_str']   = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(fdb['ctime']))
        fdb['mtime_str']   = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(fdb['mtime']))

        if fdb['ext'] in ['.par2', '.chk', '.json', '.lst']:
            for k in ['ext', 'path', 'ctime_str', 'mtime_str', 'pretty_size']:
                fdb[k] = '<span class="lowlight">%s</span>' % fdb[k]
            s = fdb['pretty_size']
            s = s.replace('gbunit', 'bunit')
            s = s.replace('mbunit', 'bunit')
            s = s.replace('kbunit', 'bunit')
            fdb['pretty_size'] = s

        fdb['index'] = i
        if i & 1 == 0:
            fdb['class'] = 'row_even'
        else:
            fdb['class'] = 'row_odd'

        total_size = total_size + fdb['size']
        i = i + 1

    i = 0
    for ddb in l['dirs']:
        ddb['ctime_str']   = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(ddb['ctime']))
        ddb['mtime_str']   = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(ddb['mtime']))

        ddb['index'] = i
        if i % 2 == 0:
            ddb['class'] = 'row_even'
        else:
            ddb['class'] = 'row_odd'

        i = i + 1

    return total_size

if __name__ == '__main__':
    opts, args = getopt.getopt(sys.argv[1:], 'o:', 'all')
    if len(args) == 0:
        print('Error: missing dir path')
        print('Usage: script.py -o file.html indir          just list files')
        print('Usage: script.py -o file.html --all indir    scan archives')
        sys.exit(-1)

    reldir   = args[0]
    absdir   = os.path.abspath(reldir)
    metafilename = os.path.split(os.path.abspath(args[0]))[1]
    title    = os.path.split(metafilename)[1]
    label    = get_volume_label(absdir)
    scan_all = False
    if label:
        print('found label of drive')
        title = label
        metafilename = label

    for k, v in opts:
        if k == '-o':
            metafilename = os.path.abspath(v)
        elif k == '--all':
            scan_all = True

    if not metafilename.endswith(SUFFIX):
        metafilename = metafilename + SUFFIX

    if not find_p7zip():
        raise BaseException('Can not find 7z.exe nor 7za.exe!')

    if not find_unrar():
        raise BaseException('Can not find UnRAR.exe nor RAR.exe!')

    if os.path.isfile(metafilename):
        print('Error: "%s" exist, stop' % metafilename)

    print('indir : "%s"' % absdir)
    print('output: "%s"' % metafilename)

    l = gen_fileslist(reldir)

    l['files-by-size'] = sorted(l['files'], key=lambda item:item['size'], reverse=True)

    total_size = pretty_db(l)
    size = pretty_size(total_size)

    if scan_all:
        print('scan *.rar and *.zip ...')
        archives = get_archive_files_list(l['files'], reldir)
        print('')
    else:
        archives = []

    tpl = Template(template)
    text = tpl.render_unicode(title=title, version=VERSION, size=size, db=l, archives=archives)
    f = open(metafilename, 'wb')
    f.write(text.encode('UTF-8'))
    f.close()


