#!/usr/bin/env python
# coding: utf-8

import time

def stringToTimestamp(timestring):
    timeSlice = timestring.split(':')
    if len(timeSlice) == 0:
        return None
    hh = int(timeSlice[0])
    mm = int(timeSlice[1])
    ss = int(timeSlice[2])
    tm = time.struct_time((2011, 3, 15, hh, mm, ss, 0, 0, 0))
    return int(time.mktime(tm))

def generatorData():
    origin = file('sh600000_成交明细_2011-03-15.xls')
    output = file('../stock.csv', 'w')

    data = []

    skip = False
    for i in origin:
        if not skip:
            skip = True
            continue
        slices = i.split('\t')
        timestring = slices[0]
        price = float(slices[1])
        timestamp = stringToTimestamp(timestring)
        data.insert(0, (timestamp, price, timestring))
    origin.close()
        
    for i in data:
        output.write(str(i[0]))
        output.write(',')
        output.write(str(i[1]))
        output.write(',')
        output.write(i[2])
        output.write('\n')
    output.close()

if __name__ == '__main__':
    generatorData()

