set multiplot

set size 0.5,.5

set xrange [.05:1]
plot "./dat/dat-ejer1/hypByTanh5.dat" with line,\
     "./dat/dat-ejer1/hypByTanh10.dat" with line,\
     "./dat/dat-ejer1/hypByTanh20.dat" with line

set origin 0.5,0.0
set size 0.5,.5
clear

set xrange [1:10]
plot "./dat/dat-ejer1/hypByTanh5.dat" with line,\
     "./dat/dat-ejer1/hypByTanh10.dat" with line,\
     "./dat/dat-ejer1/hypByTanh20.dat" with line

unset multiplot

