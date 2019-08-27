#!/usr/bin/gnuplot

#PostScript
set terminal postscript landscape enhanced color dashed lw 1 "DejaVuSans" 12
set output "process_perfs.ps"

set xlabel  'frame size (B)'
set ylabel  'data rate (MB/s)' tc lt 1
set ytics  nomirror tc lt 1
set y2label 'frame rate (1/s)' tc lt 2
set y2tics nomirror tc lt 2

plot 'process_perfs.dat' u 2:6  w lp title 'data storing rate' linetype 1, 'process_perfs.dat' u 2:($1/$4)  w lp title 'frame rate' linetype 2 axes x1y2

#PortableNetworkGraphics
set terminal png size 1024,512
set output "process_perfs.png"
replot

