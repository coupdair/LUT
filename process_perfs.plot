#!/usr/bin/gnuplot

#PostScript
set terminal postscript landscape enhanced color dashed lw 1 "DejaVuSans" 12
set output "process_perfs.ps"

set xlabel 'frame size (B)'
set ylabel 'data rate (MB/s)'
plot 'process_perfs.dat' u 1:5  w lp title 'data storing rate'


set xlabel 'frame size (B)'
set ylabel 'elapsed time (s)'
plot 'process_perfs.dat' u 1:3  w lp title 'elapsed time'

