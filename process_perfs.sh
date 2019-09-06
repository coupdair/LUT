#!/bin/bash

#fn=123456
fnpc=12345
bnpc=8
gpu=--use-GPU
pnl='1 2'
#gpu=
#pnl='1 2 4 8 16'

#run program for several frame sizes
#for fs in `cat frames.txt`
fs=4096
for pn in $pnl
do
  make clear
  ((tn=pn+2))
  ((fn=pn*fnpc))
  ((bn=pn*bnpc))
  echo frame $fs
  echo processing thread $pn
  time ./process -s $fs -c $tn -b $bn $gpu -n $fn -o /media/temp/samples/sample.cimg -r /media/temp/results/sample.cimg &>/dev/null #real: elapsed time
  du -shc /media/temp/samples/ #total: stored data size
done 2>&1 | grep -e frame -e thread -e real -e total | tee process_perfs.txt

#lines to one frame per line
#for i in `cat frames.txt`
for pn in $pnl
do
  s=`grep "thread $pn" -B 1 -A 2 process_perfs.txt`
  f=`echo $s | cut -d' ' -f2`
  p=`echo $s | cut -d' ' -f5`
  t=`echo $s | cut -d' ' -f7 | sed 's/m/min+/;s/\./s/' | head -c -5`
  g=`echo $s | cut -d' ' -f8`'B'
  r=`units "$g/($t)" MB/s -t | cut -d'.' -f1`
  ts=`units "$t" s -t | cut -d'.' -f1`
  echo $pn'pc' $bnpc'cbpc' $f'BoF' $t $ts's' $g $r'MB/s'
done > process_perfs.dat

