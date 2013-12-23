set xrange [1:50]
set terminal jpeg
set output "./img/ejer1/logByAuto-2.jpg"

plot "./dat/dat-ejer1/logByAuto-5.dat" title "P = 5" with line,\
     "./dat/dat-ejer1/logByAuto-10.dat" title "P = 10" with line,\
     "./dat/dat-ejer1/logByAuto-20.dat" title "P = 20" with line

