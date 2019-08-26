#!/bin/bash

#run program for several frame sizes
for i in `cat frames.txt`
do
  make clear
  echo frame
  time ./process -s  -n 123456 -o /media/temp/samples/sample.cimg -r /media/temp/results/sample.cimg &>/dev/null
  du -shc /media/temp/samples/
done 2>&1 | grep -e frame -e real -e total > process_perfs.txt

#lines to one frame per line
for i in `cat frames.txt`
do
  s=`grep $i -A 2 process_perfs.txt`
  f=`echo $s | cut -d' ' -f2`
  t=`echo $s | cut -d' ' -f4 | sed 's/m/min+/;s/\./s/' | head -c -5`
  g=`echo $s | cut -d' ' -f5`'B'
  r=`units "$g/($t)" MB/s -t | cut -d'.' -f1`
  echo $f $t $g $r'MB/s'
done > process_perfs.dat
