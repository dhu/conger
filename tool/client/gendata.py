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
    output = file('stock.csv', 'w')

    data = []

    # 看了半天，这里的 skip 只是为了跳过原始数据的第一行的 title
    skip = False
    for i in origin:
        if not skip:
            skip = True
            continue
        slices = i.split('\t')
        timestring = slices[0]
        price = float(slices[1])
        volume = slices[3]
        amount = slices[4]
        bid_ask = ''
        kindstring = slices[5].strip();
        if kindstring == '买盘':
            bid_ask = 'b'
        elif kindstring == '卖盘':
            bid_ask = 'a'
        else:
            bid_ask = 'n'

        timestamp = stringToTimestamp(timestring)
        data.insert(0, (timestamp, price, volume, amount, bid_ask, timestring))
    origin.close()
        
    for i in data:
        output.write(str(i[0]))
        output.write(',')
        output.write(str(i[1]))
        output.write(',')
        output.write(i[2])
        output.write(',')
        output.write(i[3])
        output.write(',')
        output.write(i[4])
        output.write(',')
        output.write(i[5])
        output.write('\n')
    output.close()

if __name__ == '__main__':
    generatorData()

