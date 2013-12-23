set xrange [1:50]
set terminal jpeg
set output "./img/ejer1/logByTanh-2.jpg"

plot "./dat/dat-ejer1/logByTanh5.dat" title "P = 5" with line,\
     "./dat/dat-ejer1/logByTanh10.dat" title "P = 10" with line,\
     "./dat/dat-ejer1/logByTanh20.dat" title "P = 20" with line

