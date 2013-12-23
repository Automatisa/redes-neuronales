set xrange [1:50]
set terminal jpeg
set output "./img/ejer1/hypByTanh-2.jpg"

plot "./dat/dat-ejer1/hypByTanh5.dat" title "P = 5" with line,\
     "./dat/dat-ejer1/hypByTanh10.dat" title "P = 10" with line,\
     "./dat/dat-ejer1/hypByTanh20.dat" title "P = 20" with line

