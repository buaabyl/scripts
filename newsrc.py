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
#
# 2012.01.22    created by buaa.byl@gmail.com
# 2016.10.24    update to python25
# 2016.10.25    fix private identification leak issuse
# 2016.11.17    support multi licenses
# 2017.01.11    using actual hashed uuid, not original value
# 2017.02.08    fix include error
# 2017.03.09    supported latex
#
import os
import sys
import uuid
import time
import hashlib
import getopt
from mako.template import Template

license_gplv2 = '''\
                  GNU GENERAL PUBLIC LICENSE
                      Version 2, June 1991

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
'''

license_lgplv3 = '''\
                  GNU LESSER GENERAL PUBLIC LICENSE
                      Version 3, 29 June 2007

Copyright (C) 2007 Free Software Foundation, Inc. 
Everyone is permitted to copy and distribute verbatim copies
of this license document, but changing it is not allowed.

This version of the GNU Lesser General Public License incorporates
the terms and conditions of version 3 of the GNU General Public
License, supplemented by the additional permissions listed below.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
'''

license_bsd = '''\
                  Berkeley Software Distribution 
                      Version 3, 1998

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of the University of California, Berkeley nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'''

license_mit = '''\
                  The MIT License

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom 
the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''


tpl_hfile = '''\
/* vim: set fileencoding=utf-8:
 *
   %for line in license:
 * ${line}
   %endfor
 *
 *  ${date}     created by {user}
 */
#ifndef ${name}
#define ${name}

#endif

'''

tpl_cfile = '''\
/* vim: set fileencoding=utf-8:
 *
   %for line in license:
 * ${line}
   %endfor
 *
 *  ${date}     created by {user}
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

int main(int argc, char* argv[])
{
    printf("hello world\\n");
    return 0;
}

'''

tpl_cppfile = '''\
/* vim: set fileencoding=utf-8:
 *
   %for line in license:
 * ${line}
   %endfor
 *
 *  ${date}     created by {user}
 */
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[])
{
    std::cout << "hello world";
    std::cout << std::endl;
    return 0;
}

'''

tpl_pyfile = '''\
#!/usr/bin/env python3
# -*- coding:utf-8 -*-
#
  %for line in license:
# ${line}
  %endfor
#
#  ${date}      created by {user}
#
import sys
import os
import re
import getopt
import glob

if __name__ == '__main__':
    print('hello world')

'''

tpl_texmakefile = '''\
all:
\txelatex -shell-escape ${filename}
'''

tpl_texheader = '''\
%# -*- coding: utf-8 -*-
%!TEX encoding = UTF-8 Unicode
'''

tpl_texfile = r'''
\documentclass{article}
\usepackage{xeCJK}
\usepackage{minted} 
\usepackage{url}

% using as \begin{ccode} ... \end(ccode}
\newminted{c}{linenos,fontsize=\normalsize}
\newminted{go}{linenos,fontsize=\normalsize}
\newminted{python}{linenos,fontsize=\normalsize}
% linenos font style
\renewcommand{\theFancyVerbLine} {
    \small\textcolor[rgb]{0.5,0.5,1.0}{
    \arabic{FancyVerbLine}}}

\title{Title}
\author{buaa.byl@gmail.com}
\begin{document}
\maketitle

% --------------------------------------
\section{Section1}

\textbf{Hello} world\footnote{notes}
\newline
Hi
\newpage

\subsection{Say hello}
% \begin{figure}[htb]
%     \centering
%     \includegraphics[width=\textwidth/2]{example.jpg}
% \end{figure}

% --------------------------------------
\end{document}

'''


def generate_uuid(fn):
    filepath = os.path.split(fn)[1]
    filename, fileext = os.path.splitext(filepath)

    #such as '57f9f97a-9a5a-11e6-85c9-000000000000'
    #last part is mac address, may private issuse!
    raw = hashlib.sha512(str(uuid.uuid1()).encode('UTF-8')).hexdigest()
    uuids = [0,0,0,0,0]
    uuids[0] = raw[:8]
    uuids[1] = raw[8:12]
    uuids[2] = raw[12:16]
    uuids[3] = raw[16:20]
    uuids[4] = raw[20:32]
    uuidstr  = '-'.join(uuids).upper()

    filename_upp = filename.upper()
    fileext_upp  = fileext.upper()
    headfilemacro = '%s%s_%s_INCLUDED_' % (filename_upp, fileext_upp, uuidstr)
    headfilemacro = headfilemacro.replace('.','_')
    headfilemacro = headfilemacro.replace('-','_')
    return headfilemacro

def print_usage(prog):
    print('usage: %s [options] newfilename' % os.path.split(prog)[1])
    print('options:')
    print('  %2s  %-22s %s' % ('', '--gpl',     'GPLv2'))
    print('  %2s  %-22s %s' % ('', '--lgpl',    'LGPLv2'))
    print('  %2s  %-22s %s' % ('', '--mit',     'MIT'))
    print('  %2s  %-22s %s' % ('', '--bsd',     'BSD'))

if __name__ == '__main__':
    opts, args = getopt.getopt(sys.argv[1:], '', ['gpl', 'lgpl', 'mit', 'bsd'])
    if len(args) == 0:
        print_usage(sys.argv[0])
        sys.exit(-1)

    support_exts = ['.c', '.cc','.cpp', '.h', '.hpp', '.py', '.tex']

    licenses = [('gpl', license_gplv2), ('lgpl', license_lgplv3), ('mit', license_mit), ('bsd', license_bsd)]
    licenses = dict((v[0], v[1].split('\n')) for v in licenses)
    license = ('gpl', licenses['gpl'])
    for k, v in opts:
        if k.startswith('--'):
            if k[2:] in licenses:
                license = (k[2:], licenses[k[2:]])
                break

    print('license: %s' % license[0].upper())

    # check file extension
    for fn in args:
        filepath = os.path.split(fn)[1]
        filename, fileext = os.path.splitext(filepath)
        if fileext not in support_exts:
            print('file type "%s" not support' % fileext)
            print('just support: ' + ' '.join(support_exts))
            sys.exit(-1)

    for fn in args:
        filepath = os.path.split(fn)[1]
        filename, fileext = os.path.splitext(filepath)
        date = time.strftime('%Y.%m.%d')

        if fileext == '.tex':
            f = open(fn, 'w')
            f.write(tpl_texheader)
            f.write('% \n')
            for line in license[1]:
                f.write('% ')
                f.write(line)
                f.write('\n')
            f.write('%% %s    created by {user}\n' % date)
            f.write(tpl_texfile)
            f.close()
            print('wrote to "%s"' % fn)

            f = open(fn + '.makefile', 'w')
            f.write(tpl_texmakefile.replace('${filename}', fn))
            f.close()
            print('wrote to "%s.makefile"' % fn)

            continue

        if fileext == '.py':
            h = Template(tpl_pyfile)
        elif fileext == '.c':
            h = Template(tpl_cfile)
        elif fileext in ['.cc', '.cpp']:
            h = Template(tpl_cppfile)
        elif fileext in ['.h', '.hpp']:
            h = Template(tpl_hfile)

        name = generate_uuid(filepath)
        text = h.render_unicode(name=name, date=date, licname=license[0], license=license[1])

        f = open(fn, 'w')
        f.write(text)
        f.close()

        print('wrote to "%s"' % fn)


