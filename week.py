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
# 2016.10.24  update to python35
#
import sys
import time
import calendar
import datetime
import ctypes
import win32console
import win32con

class ConsoleColor:
    def __init__(self):
        self.BLACK        = 0
        self.BLUE         = 1
        self.DARK_GREEN   = 2
        self.LIGHT_BLUE   = 3
        self.RED          = 4
        self.PURPLE       = 5
        self.ORANGE       = 6
        self.GREY         = 7
        self.DARKER_GREY  = 8
        self.MEDIUM_BLUE  = 9
        self.LIGHT_GREEN  = 10
        self.TEAL         = 11
        self.RED_ORANGE   = 12
        self.LIGHT_PURPLE = 13
        self.YELLOW       = 14
        self.WHITE        = 15
        self.stdout = win32console.GetStdHandle(win32console.STD_OUTPUT_HANDLE)

    def write(self, color, s):
        self.stdout.SetConsoleTextAttribute(color)
        self.stdout.WriteConsole(s)

def print_weeks():
    currentTime = time.time()

    #get current timestamp
    formatedvalue = time.localtime(currentTime)
    year        = formatedvalue[0]
    month       = formatedvalue[1]
    day         = formatedvalue[2]
    timesyear   = time.strftime('%Y',time.localtime(currentTime))
    timesmonth  = time.strftime('%m',time.localtime(currentTime))
    timesday    = time.strftime('%d',time.localtime(currentTime))

    #US format: sunday is first day, range [00, 53],
    #but all online website let first day is week-1.
    #:(
    currWeekNo  = time.strftime('%U',time.localtime(currentTime))

    #Calc number of week
    difdays     = (datetime.datetime(year,12,31) - datetime.datetime(year,month,day)).days
    endOfTime   = currentTime + difdays * 24 * 60 * 60
    endWeekNo   = time.strftime('%U',time.localtime(endOfTime))

    cc = ConsoleColor()
    cc.write(cc.DARKER_GREY,    " Python %d.%d\n" % (sys.version_info.major, sys.version_info.minor))
    cc.write(cc.DARKER_GREY,    " Version: 2016.10.25\n")
    cc.write(cc.WHITE,          ' **********************\n')
    cc.write(cc.WHITE,          ' * ')
    cc.write(cc.YELLOW,         '%02s' % currWeekNo)
    cc.write(cc.DARKER_GREY,    '/')
    cc.write(cc.YELLOW,         '%02s' % endWeekNo)
    cc.write(cc.WHITE,          ' , ')
    cc.write(cc.LIGHT_GREEN,    '%s' % timesyear)
    cc.write(cc.DARKER_GREY,    '/')
    cc.write(cc.LIGHT_GREEN,    '%s' % timesmonth)
    cc.write(cc.DARKER_GREY,    '/')
    cc.write(cc.LIGHT_GREEN,    '%s' % timesday)
    cc.write(cc.WHITE,          ' *\n')
    cc.write(cc.WHITE,          ' **********************')
    cc.write(cc.GREY,           '\n') 

if __name__ == '__main__':
    print_weeks()


